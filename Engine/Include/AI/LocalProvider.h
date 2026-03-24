#pragma once

#include "AI/BaseAgentProvider.h"

namespace Nova {

    class LocalProvider : public BaseAgentProvider {
    public:
        LocalProvider(const std::string& endpoint = "http://localhost:11434/v1/chat/completions") 
            : BaseAgentProvider("", "llama3"), m_Endpoint(endpoint) {}

    protected:
        std::string GetProviderName() const override { return "Local LLM"; }
        std::string GetHost() const override { return "localhost"; }
        std::string GetPath() const override { return "/v1/chat/completions"; }
        std::string GetHeaders() const override { return ""; }
        int GetPort() const override { return 11434; }
        bool IsSecure() const override { return false; }

        std::string PrepareRequestBody(const std::vector<AgentMessage>& messages) const override {
            nlohmann::json req;
            req["model"] = m_Model;
            nlohmann::json jMessages = nlohmann::json::array();
            for (auto const& msg : messages) {
                jMessages.push_back({{"role", msg.Role}, {"content", msg.Content}});
            }
            req["messages"] = jMessages;
            return req.dump();
        }

        void ParseResponse(const std::string& response, std::function<void(const std::string&)> callback) const override {
            try {
                auto resJson = nlohmann::json::parse(response);
                if (resJson.contains("choices") && !resJson["choices"].empty()) {
                    std::string text = resJson["choices"][0]["message"]["content"];
                    callback(text);
                } else {
                    callback("Error: Unexpected Local LLM response.");
                }
            } catch (...) {
                callback("Error: Failed to parse Local LLM response.");
            }
        }

    private:
        std::string m_Endpoint;
    };

} // namespace Nova
