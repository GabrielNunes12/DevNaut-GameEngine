#pragma once

#include "AI/BaseAgentProvider.h"

namespace Nova {

    class ClaudeProvider : public BaseAgentProvider {
    public:
        ClaudeProvider(const std::string& apiKey) : BaseAgentProvider(apiKey, "claude-3-5-sonnet-20240620") {}

    protected:
        std::string GetProviderName() const override { return "Anthropic Claude"; }
        std::string GetHost() const override { return "api.anthropic.com"; }
        std::string GetPath() const override { return "/v1/messages"; }
        std::string GetHeaders() const override { return "x-api-key: " + m_ApiKey + "\r\nanthropic-version: 2023-06-01\r\n"; }

        std::string PrepareRequestBody(const std::vector<AgentMessage>& messages) const override {
            nlohmann::json req;
            req["model"] = m_Model;
            req["max_tokens"] = 1024;
            nlohmann::json jMessages = nlohmann::json::array();
            for (auto const& msg : messages) {
                if (msg.Role == "system") {
                    req["system"] = msg.Content;
                } else {
                    jMessages.push_back({{"role", msg.Role}, {"content", msg.Content}});
                }
            }
            req["messages"] = jMessages;
            return req.dump();
        }

        void ParseResponse(const std::string& response, std::function<void(const std::string&)> callback) const override {
            try {
                auto resJson = nlohmann::json::parse(response);
                if (resJson.contains("content") && !resJson["content"].empty()) {
                    std::string text = resJson["content"][0]["text"];
                    callback(text);
                } else if (resJson.contains("error")) {
                    callback("Claude Error: " + resJson["error"]["message"].get<std::string>());
                } else {
                    callback("Error: Unexpected Claude response.");
                }
            } catch (...) {
                callback("Error: Failed to parse Claude response.");
            }
        }
    };

} // namespace Nova
