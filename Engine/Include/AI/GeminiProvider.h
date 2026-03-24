#pragma once

#include "AI/BaseAgentProvider.h"

namespace Nova {

    class GeminiProvider : public BaseAgentProvider {
    public:
        GeminiProvider(const std::string& apiKey) : BaseAgentProvider(apiKey, "gemini-2.5-flash") {}

    protected:
        std::string GetProviderName() const override { return "Google Gemini"; }
        std::string GetHost() const override { return "generativelanguage.googleapis.com"; }
        std::string GetPath() const override { return "/v1beta/models/" + m_Model + ":generateContent"; }
        std::string GetHeaders() const override { return "x-goog-api-key: " + m_ApiKey + "\r\nAccept: */*\r\n"; }

        std::string PrepareRequestBody(const std::vector<AgentMessage>& messages) const override {
            nlohmann::json req;
            nlohmann::json contents = nlohmann::json::array();
            
            for (auto const& msg : messages) {
                if (msg.Role == "system") {
                    req["system_instruction"] = {{"parts", {{{"text", msg.Content}}}}};
                } else {
                    nlohmann::json content;
                    content["role"] = (msg.Role == "assistant") ? "model" : "user";
                    content["parts"] = {{{"text", msg.Content}}};
                    // Ensure we don't send consecutive user/model roles (Gemini requirement)
                    if (!contents.empty() && contents.back()["role"] == content["role"]) {
                        contents.back()["parts"][0]["text"] = contents.back()["parts"][0]["text"].get<std::string>() + "\n" + msg.Content;
                    } else {
                        contents.push_back(content);
                    }
                }
            }
            req["contents"] = contents;
            return req.dump();
        }

        void ParseResponse(const std::string& response, std::function<void(const std::string&)> callback) const override {
            try {
                auto resJson = nlohmann::json::parse(response);
                if (resJson.contains("candidates") && !resJson["candidates"].empty()) {
                    std::string text = resJson["candidates"][0]["content"]["parts"][0]["text"];
                    callback(text);
                } else if (resJson.contains("error")) {
                    callback("Gemini Error: " + resJson["error"]["message"].get<std::string>());
                } else {
                    callback("Error: Unexpected Gemini response structure.");
                }
            } catch (...) {
                callback("Error: Failed to parse Gemini response. (Invalid JSON)");
            }
        }
    };

} // namespace Nova
