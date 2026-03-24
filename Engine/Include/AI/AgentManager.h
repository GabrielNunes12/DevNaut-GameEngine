#pragma once

#include "AI/IAgentProvider.h"
#include "AI/AgentRoleRegistry.h"
#include <memory>
#include <vector>
#include <string>

namespace Nova {

    class AgentManager {
    public:
        static AgentManager& Instance() {
            static AgentManager instance;
            return instance;
        }

        void SetProvider(std::shared_ptr<IAgentProvider> provider) { m_Provider = provider; }
        IAgentProvider* GetProvider() const { return m_Provider.get(); }

        void SetActiveRole(const std::string& roleName) { m_ActiveRole = roleName; }
        std::string GetActiveRole() const { return m_ActiveRole; }

        // Send a message to the active provider with the active role's system prompt
        void Ask(const std::string& userPrompt, std::function<void(const std::string&)> callback);

    private:
        AgentManager() = default;

        std::shared_ptr<IAgentProvider> m_Provider;
        std::string m_ActiveRole = "Gameplay Programmer";
        std::vector<AgentMessage> m_ChatHistory;
    };

} // namespace Nova
