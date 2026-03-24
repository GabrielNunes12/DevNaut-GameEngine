#pragma once

#include "AI/BaseAgentProvider.h"

namespace Nova {

    class OpenAIProvider : public BaseAgentProvider {
    public:
        OpenAIProvider(const std::string& apiKey) : BaseAgentProvider(apiKey, "gpt-4o") {}

    protected:
        std::string GetProviderName() const override { return "OpenAI"; }
        std::string GetHost() const override { return "api.openai.com"; }
        std::string GetPath() const override { return "/v1/chat/completions"; }
        std::string GetHeaders() const override { return "Authorization: Bearer " + m_ApiKey + "\r\n"; }

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
                } else if (resJson.contains("error")) {
                    callback("OpenAI Error: " + resJson["error"]["message"].get<std::string>());
                } else {
                    callback("Error: Unexpected OpenAI response.");
                }
            } catch (...) {
                callback("Error: Failed to parse OpenAI response.");
            }
        }
    };

} // namespace Nova
