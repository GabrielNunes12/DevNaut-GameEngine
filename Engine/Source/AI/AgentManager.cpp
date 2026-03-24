#include "AI/AgentManager.h"
#include "Scene/Scene.h"
#include "Core/Logger.h"
#include <thread>
#include <iostream>

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

        // Wrap the callback to intercept commands
        auto wrappedCallback = [this, callback](const std::string& response) {
            this->ProcessCommand(response);
            callback(response);
        };

        // Run async
        std::thread([this, messages, wrappedCallback]() {
            m_Provider->SendRequest(messages, wrappedCallback);
        }).detach();
    }

    void AgentManager::ProcessCommand(const std::string& response) {
        if (!m_Scene) return;

        // Simple parser for NOVA_CMD:ACTION[param][param]
        size_t cmdPos = response.find("NOVA_CMD:");
        if (cmdPos == std::string::npos) return;

        std::string cmdPart = response.substr(cmdPos + 9);
        
        // Spawn Command: SPAWN[type][name]
        if (cmdPart.find("SPAWN") == 0) {
            size_t typeStart = cmdPart.find('[');
            size_t typeEnd = cmdPart.find(']', typeStart);
            size_t nameStart = cmdPart.find('[', typeEnd);
            size_t nameEnd = cmdPart.find(']', nameStart);

            if (typeStart != std::string::npos && typeEnd != std::string::npos && 
                nameStart != std::string::npos && nameEnd != std::string::npos) {
                
                std::string type = cmdPart.substr(typeStart + 1, typeEnd - typeStart - 1);
                std::string name = cmdPart.substr(nameStart + 1, nameEnd - nameStart - 1);

                if (type == "cube") {
                    m_Scene->AddEntity(name, EntityType::Mesh);
                }
            }
        }
        else if (cmdPart.find("DELETE") == 0) {
            size_t nameStart = cmdPart.find('[');
            size_t nameEnd = cmdPart.find(']', nameStart);
            if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                std::string name = cmdPart.substr(nameStart + 1, nameEnd - nameStart - 1);
                auto* entity = m_Scene->FindByName(name);
                if (entity) {
                    m_Scene->RemoveEntity(entity->GetId());
                }
            }
        }
    }

} // namespace Nova
