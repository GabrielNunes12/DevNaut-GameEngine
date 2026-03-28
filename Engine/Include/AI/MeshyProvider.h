#pragma once

#include "AI/BaseAgentProvider.h"
#include <nlohmann/json.hpp>

namespace Nova {

    /**
     * @brief AI Provider for Meshy.ai (3D Mesh Generation)
     */
    class MeshyProvider : public BaseAgentProvider {
    public:
        MeshyProvider(const std::string& apiKey)
            : BaseAgentProvider(apiKey, "preview") {}

        void SendRequest(const std::vector<AgentMessage>& messages, std::function<void(const std::string&)> callback) override {
            std::string prompt = "";
            for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
                if (it->Role == "user") {
                    prompt = it->Content;
                    break;
                }
            }

            if (prompt.empty()) {
                callback("Error: No prompt provided for 3D generation.");
                return;
            }

            nlohmann::json body = {
                {"prompt", prompt},
                {"mode", m_Model}, // Using m_Model as the mode (preview/refine)
                {"art_style", "realistic"}
            };

            std::string response;
            if (PerformHttpRequest(GetHost(), GetPath(), body.dump(), GetHeaders(), response)) {
                try {
                    auto json = nlohmann::json::parse(response);
                    if (json.contains("result")) {
                        std::string taskId = json["result"];
                        callback("Meshy Task Created: " + taskId + " (Generating...)");
                    } else if (json.contains("message")) {
                         callback("Meshy API: " + json["message"].get<std::string>());
                    } else {
                        callback("Meshy Output: " + response);
                    }
                } catch (...) {
                    callback("Meshy Parse Error: " + response);
                }
            } else {
                callback("Error: Meshy connection failed.");
            }
        }

        std::string GetName() const override { return "Meshy AI"; }

    protected:
        // These are required by BaseAgentProvider but we override SendRequest anyway.
        // We'll provide defaults to satisfy the compiler.
        std::string PrepareRequestBody(const std::vector<AgentMessage>&) const override { return ""; }
        void ParseResponse(const std::string&, std::function<void(const std::string&)>) const override {}
        
        std::string GetHost() const override { return "api.meshy.ai"; }
        std::string GetPath() const override { return "/openapi/v2/text-to-3d"; }
        std::string GetHeaders() const override { 
            return "Authorization: Bearer " + m_ApiKey; 
        }
        std::string GetProviderName() const override { return "Meshy"; }
    };

} // namespace Nova
