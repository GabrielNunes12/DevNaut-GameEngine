#include "AI/AgentManager.h"
#include "Scene/Scene.h"
#include "Core/Logger.h"
#include "Physics/Collider.h"
#include <thread>
#include <sstream>
#include <vector>

namespace Nova {

    void AgentManager::Ask(const std::string& userPrompt, std::function<void(const std::string&)> callback) {
        if (!m_Provider) {
            callback("Error: No AI Provider configured.");
            return;
        }

        std::vector<AgentMessage> messages;
        auto const& roles = AgentRoleRegistry::GetRoles();
        if (roles.count(m_ActiveRole)) {
            messages.push_back({"system", roles.at(m_ActiveRole).SystemPrompt});
        }
        messages.push_back({"user", userPrompt});

        auto wrappedCallback = [this, callback](const std::string& response) {
            this->ProcessCommand(response);
            callback(response);
        };

        std::thread([this, messages, wrappedCallback]() {
            m_Provider->SendRequest(messages, wrappedCallback);
        }).detach();
    }

    void AgentManager::ProcessCommand(const std::string& response) {
        if (!m_Scene) return;

        size_t searchPos = 0;
        while ((searchPos = response.find("NOVA_CMD:", searchPos)) != std::string::npos) {
            size_t lineEnd = response.find('\n', searchPos);
            std::string line = (lineEnd == std::string::npos) ? response.substr(searchPos) : response.substr(searchPos, lineEnd - searchPos);
            searchPos += line.length(); 

            std::string cmd = line.substr(9); // Strip NOVA_CMD:
            
            auto GetParam = [](const std::string& s, int index) -> std::string {
                size_t start = 0;
                for (int i = 0; i <= index; ++i) {
                    start = s.find('[', start);
                    if (start == std::string::npos) return "";
                    start++; 
                }
                size_t end = s.find(']', start);
                if (end == std::string::npos) return "";
                return s.substr(start, end - start);
            };

            if (cmd.find("SPAWN") == 0) {
                std::string type = GetParam(cmd, 0);
                std::string name = GetParam(cmd, 1);
                std::string posStr = GetParam(cmd, 2);
                std::string scaleStr = GetParam(cmd, 3);

                if (type == "cube") {
                    Entity* e = m_Scene->AddEntity(name, EntityType::Mesh);
                    if (e) {
                        float x = 0, y = 0, z = 0;
                        sscanf(posStr.c_str(), "%f,%f,%f", &x, &y, &z);
                        e->GetTransform().Position = {x, y, z};
                        
                        if (!scaleStr.empty()) {
                            float s = (float)atof(scaleStr.c_str());
                            if (s > 0) e->GetTransform().Scale = {s, s, s};
                        }
                    }
                }
            }
            else if (cmd.find("MOVE") == 0) {
                std::string name = GetParam(cmd, 0);
                std::string posStr = GetParam(cmd, 1);
                Entity* e = m_Scene->FindByName(name);
                if (e) {
                    float x = 0, y = 0, z = 0;
                    sscanf(posStr.c_str(), "%f,%f,%f", &x, &y, &z);
                    e->GetTransform().Position = {x, y, z};
                }
            }
            else if (cmd.find("PHYSICS") == 0) {
                std::string name = GetParam(cmd, 0);
                Entity* e = m_Scene->FindByName(name);
                if (e) {
                    e->AddRigidbody();
                    e->AddCollider(ColliderType::Box);
                    NOVA_LOG_INFO("AI Action: Physics ENABLED for entity '{}'.", name);
                }
            }
            else if (cmd.find("DELETE") == 0) {
                std::string name = GetParam(cmd, 0);
                Entity* e = m_Scene->FindByName(name);
                if (e) m_Scene->RemoveEntity(e->GetId());
            }
            else if (cmd.find("CLEAR") == 0) {
                m_Scene->Clear();
                NOVA_LOG_INFO("AI Action: Scene CLEARED for rebuild.");
            }
        }
    }

} // namespace Nova
