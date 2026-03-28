#pragma once

#include "AI/BaseAgentProvider.h"
#include <nlohmann/json.hpp>

namespace Nova {

    /**
     * @brief AI Provider for Tripo AI (High-speed 3D Model Generation)
     */
    class TripoProvider : public BaseAgentProvider {
    public:
        TripoProvider(const std::string& apiKey)
            : BaseAgentProvider(apiKey, "text_to_model") {}

        void SendRequest(const std::vector<AgentMessage>& messages, std::function<void(const std::string&)> callback) override {
            std::string prompt = "";
            for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
                if (it->Role == "user") {
                    prompt = it->Content;
                    break;
                }
            }

            if (prompt.empty()) {
                callback("Error: No prompt provided for Tripo generation.");
                return;
            }

            nlohmann::json body = {
                {"type", m_Model}, // "text_to_model"
                {"prompt", prompt}
            };

            std::string response;
            if (PerformHttpRequest("api.tripo3d.ai", "/v1/task", body.dump(), GetHeaders(), response)) {
                try {
                    auto json = nlohmann::json::parse(response);
                    if (json.contains("data") && json["data"].contains("task_id")) {
                        std::string taskId = json["data"]["task_id"];
                        callback("Tripo Task Created: " + taskId + " (Generating in seconds...)");
                    } else if (json.contains("message")) {
                        callback("Tripo API: " + json["message"].get<std::string>());
                    } else {
                        callback("Tripo Output: " + response);
                    }
                } catch (...) {
                    callback("Tripo Parse Error: " + response);
                }
            } else {
                callback("Error: Tripo connection failed.");
            }
        }

        std::string GetName() const override { return "Tripo AI"; }

    protected:
        std::string PrepareRequestBody(const std::vector<AgentMessage>&) const override { return ""; }
        void ParseResponse(const std::string&, std::function<void(const std::string&)>) const override {}
        
        std::string GetHost() const override { return "api.tripo3d.ai"; }
        std::string GetPath() const override { return "/v1/task"; }
        std::string GetHeaders() const override { 
            return "Authorization: Bearer " + m_ApiKey; 
        }
        std::string GetProviderName() const override { return "Tripo"; }
    };

} // namespace Nova
