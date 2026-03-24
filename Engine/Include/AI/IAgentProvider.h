#pragma once

#include <string>
#include <functional>
#include <vector>

namespace Nova {

    struct AgentMessage {
        std::string Role;    // "user" or "assistant" or "system"
        std::string Content;
    };

    class IAgentProvider {
    public:
        virtual ~IAgentProvider() = default;

        // Asynchronous request to the LLM
        virtual void SendRequest(const std::vector<AgentMessage>& messages, 
                                 std::function<void(const std::string&)> callback) = 0;

        virtual void SetModel(const std::string& model) = 0;
        virtual std::string GetName() const = 0;
    };

} // namespace Nova
