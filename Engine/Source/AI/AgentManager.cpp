#include "AI/AgentManager.h"
#include <thread>

namespace Nova {

    void AgentManager::Ask(const std::string& userPrompt, std::function<void(const std::string&)> callback) {
        if (!m_Provider) {
            callback("Error: No AI Provider configured.");
            return;
        }

        // Prepare messages
        std::vector<AgentMessage> messages;
        
        // Add System Prompt
        auto const& roles = AgentRoleRegistry::GetRoles();
        if (roles.count(m_ActiveRole)) {
            messages.push_back({"system", roles.at(m_ActiveRole).SystemPrompt});
        }

        // Add history (optional, for now just the current prompt)
        messages.push_back({"user", userPrompt});

        // Run async
        std::thread([this, messages, callback]() {
            m_Provider->SendRequest(messages, callback);
        }).detach();
    }

} // namespace Nova
