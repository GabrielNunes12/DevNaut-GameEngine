#pragma once

#include "AI/BaseAgentProvider.h"
#include <nlohmann/json.hpp>

namespace Nova {

    /**
     * @brief AI Provider for Hugging Face (OpenAI-Compatible Inference Router)
     * Provides a completely free, open-source path using the latest HF infrastructure (2025/2026).
     */
    class HuggingFaceProvider : public BaseAgentProvider {
    public:
        HuggingFaceProvider(const std::string& apiKey)
            : BaseAgentProvider(apiKey, "mistralai/Mistral-7B-Instruct-v0.3") {}

        void SendRequest(const std::vector<AgentMessage>& messages, std::function<void(const std::string&)> callback) override {
            // Force Chat completions via Router
            std::string body = PrepareRequestBody(messages);
            std::string response;
            
            NOVA_LOG_INFO("HF Router Request for Model: {}", m_Model);
            
            if (PerformHttpRequest(GetHost(), GetPath(), body, GetHeaders(), response)) {
                ParseResponse(response, callback);
            } else {
                callback("Error: Hugging Face Router connection failed. Ensure your model (e.g., " + m_Model + ") is an LLM and supported by the router.");
            }
        }

        std::string GetName() const override { return "Hugging Face (Router)"; }

    protected:
        // Use the new Inference Router (OpenAI compatible)
        std::string GetHost() const override { return "router.huggingface.co"; }
        std::string GetPath() const override { return "/v1/chat/completions"; }
        
        std::string GetHeaders() const override { 
            return "Authorization: Bearer " + m_ApiKey + "\nContent-Type: application/json"; 
        }
        
        std::string GetProviderName() const override { return "HuggingFace"; }

        std::string PrepareRequestBody(const std::vector<AgentMessage>& messages) const override {
            nlohmann::json body;
            body["model"] = m_Model;
            
            nlohmann::json msgsArray = nlohmann::json::array();
            for (auto const& m : messages) {
                msgsArray.push_back({ {"role", m.Role}, {"content", m.Content} });
            }
            body["messages"] = msgsArray;
            body["max_tokens"] = 512;
            
            return body.dump();
        }

        void ParseResponse(const std::string& response, std::function<void(const std::string&)> callback) const override {
            try {
                auto json = nlohmann::json::parse(response);
                if (json.contains("choices") && !json["choices"].empty()) {
                    auto const& msg = json["choices"][0]["message"];
                    callback(msg["content"].get<std::string>());
                } else if (json.contains("error")) {
                    callback("HF Router Error: " + json["error"]["message"].get<std::string>());
                } else if (json.is_array() && !json.empty() && json[0].contains("generated_text")) {
                     // Fallback for some models still returning legacy format on router
                    callback(json[0]["generated_text"].get<std::string>());
                } else {
                    callback("HF Router Unexpected: " + response);
                }
            } catch (...) {
                callback("HF Router Parse Error: " + response);
            }
        }
    };

} // namespace Nova
