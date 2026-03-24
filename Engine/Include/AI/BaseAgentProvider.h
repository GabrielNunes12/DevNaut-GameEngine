#pragma once

#include "AI/IAgentProvider.h"
#include <windows.h>
#include <winhttp.h>
#include <nlohmann/json.hpp>
#include "Core/Logger.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

namespace Nova {

    class BaseAgentProvider : public IAgentProvider {
    public:
        BaseAgentProvider(const std::string& apiKey, const std::string& defaultModel) 
            : m_ApiKey(apiKey), m_Model(defaultModel) {
            m_ApiKey.erase(std::remove(m_ApiKey.begin(), m_ApiKey.end(), '\r'), m_ApiKey.end());
            m_ApiKey.erase(std::remove(m_ApiKey.begin(), m_ApiKey.end(), '\n'), m_ApiKey.end());
        }

        void SendRequest(const std::vector<AgentMessage>& messages, 
                         std::function<void(const std::string&)> callback) override {
            
            std::string body = PrepareRequestBody(messages);
            std::string response;
            
            if (PerformHttpRequest(GetHost(), GetPath(), body, GetHeaders(), response)) {
                NOVA_LOG_INFO("AI Raw Response: {}", response);
                ParseResponse(response, callback);
            } else {
                callback("Error: HTTPS request failed (WinHTTP).");
            }
        }

        void SetModel(const std::string& model) override { m_Model = model; }
        std::string GetName() const override { return GetProviderName() + " (" + m_Model + ")"; }

    protected:
        virtual std::string PrepareRequestBody(const std::vector<AgentMessage>& messages) const = 0;
        virtual void ParseResponse(const std::string& response, std::function<void(const std::string&)> callback) const = 0;
        virtual std::string GetHost() const = 0;
        virtual std::string GetPath() const = 0;
        virtual std::string GetHeaders() const = 0;
        virtual std::string GetProviderName() const = 0;
        virtual int GetPort() const { return INTERNET_DEFAULT_HTTPS_PORT; }
        virtual bool IsSecure() const { return true; }

        bool PerformHttpRequest(const std::string& host, const std::string& path, const std::string& body, const std::string& customHeaders, std::string& outResponse) const {
            // Sanitize API key
            std::string sanitizedKey = m_ApiKey;
            sanitizedKey.erase(std::remove(sanitizedKey.begin(), sanitizedKey.end(), '\r'), sanitizedKey.end());
            sanitizedKey.erase(std::remove(sanitizedKey.begin(), sanitizedKey.end(), '\n'), sanitizedKey.end());

            HINTERNET hSession = WinHttpOpen(L"Mozilla/5.0", WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
            if (!hSession) return false;

            DWORD dwTls = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 | WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
            WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &dwTls, sizeof(dwTls));

            std::wstring wHost(host.begin(), host.end());
            HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(), (INTERNET_PORT)GetPort(), 0);
            if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

            std::wstring wPath(path.begin(), path.end());
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
            if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

            // Helper to add a single header safely
            auto AddOneHeader = [&](std::string h) {
                h.erase(std::remove(h.begin(), h.end(), '\r'), h.end());
                h.erase(std::remove(h.begin(), h.end(), '\n'), h.end());
                if (h.empty()) return;
                
                int wlen = MultiByteToWideChar(CP_UTF8, 0, h.c_str(), -1, NULL, 0);
                std::vector<wchar_t> wbuf(wlen);
                MultiByteToWideChar(CP_UTF8, 0, h.c_str(), -1, wbuf.data(), wlen);
                if (!WinHttpAddRequestHeaders(hRequest, wbuf.data(), (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
                    NOVA_LOG_ERROR("WinHttpAddRequestHeaders failed for [{}]: Error {}", h, GetLastError());
                }
            };

            AddOneHeader("Content-Type: application/json");

            std::stringstream ss(customHeaders);
            std::string line;
            while (std::getline(ss, line)) {
                AddOneHeader(line);
            }

            NOVA_LOG_INFO("Requesting Path: {}", path);
            
            BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, (DWORD)body.length(), 0);
            
            if (bResults && !body.empty()) {
                DWORD dwWritten = 0;
                bResults = WinHttpWriteData(hRequest, body.c_str(), (DWORD)body.length(), &dwWritten);
            }
            
            if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);

            if (bResults) {
                DWORD statusCode = 0;
                DWORD statusCodeSize = sizeof(statusCode);
                WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX);
                NOVA_LOG_INFO("AI Request Status Code: {}", statusCode);

                DWORD dwSize = 0;
                while (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize > 0) {
                    std::vector<char> buffer(dwSize);
                    DWORD dwDownloaded = 0;
                    if (WinHttpReadData(hRequest, (LPVOID)buffer.data(), dwSize, &dwDownloaded)) {
                        outResponse.append(buffer.data(), dwDownloaded);
                    }
                }
            } else {
                NOVA_LOG_ERROR("WinHttpSendRequest/ReceiveResponse failed. Error: {}", GetLastError());
            }

            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return bResults;
        }

        std::string m_ApiKey;
        std::string m_Model;
    };

} // namespace Nova
