#pragma once

#include "AI/IAgentProvider.h"
#include "AI/AgentRoleRegistry.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace Nova {

    class Scene;

    class AgentManager {
    public:
        static AgentManager& Instance() {
            static AgentManager instance;
            return instance;
        }

        void SetProvider(std::shared_ptr<IAgentProvider> provider) { m_Provider = provider; }
        IAgentProvider* GetProvider() const { return m_Provider.get(); }

        void SetMeshyProvider(std::shared_ptr<IAgentProvider> meshy) { m_MeshyProvider = meshy; }
        IAgentProvider* GetMeshyProvider() const { return m_MeshyProvider.get(); }

        void SetTripoProvider(std::shared_ptr<IAgentProvider> tripo) { m_TripoProvider = tripo; }
        IAgentProvider* GetTripoProvider() const { return m_TripoProvider.get(); }

        void SetHFProvider(std::shared_ptr<IAgentProvider> hf) { m_HFProvider = hf; }
        IAgentProvider* GetHFProvider() const { return m_HFProvider.get(); }

        void SetScene(Scene* scene) { m_Scene = scene; }
        Scene* GetScene() const { return m_Scene; }

        void SetActiveRole(const std::string& roleName) { m_ActiveRole = roleName; }
        std::string GetActiveRole() const { return m_ActiveRole; }

        // Send a message to the active provider with the active role's system prompt
        // Allows for role overriding (agentic sub-calls)
        void Ask(const std::string& userPrompt, std::function<void(const std::string&)> callback, const std::string& roleOverride = "");

    private:
        AgentManager() = default;
        // Process NOVA_CMD commands from standard AI responses
        void ProcessCommand(const std::string& response, std::function<void(const std::string&)> originalCallback);

        Scene* m_Scene = nullptr;

        std::shared_ptr<IAgentProvider> m_Provider;
        std::shared_ptr<IAgentProvider> m_MeshyProvider;
        std::shared_ptr<IAgentProvider> m_TripoProvider;
        std::shared_ptr<IAgentProvider> m_HFProvider;
        
        std::string m_ActiveRole = "Gameplay Programmer";
        std::vector<AgentMessage> m_ChatHistory;
    };

} // namespace Nova
