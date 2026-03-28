#include "AI/AgentManager.h"
#include "Scene/Scene.h"
#include "Core/Logger.h"
#include "Physics/Collider.h"
#include <thread>
#include <sstream>
#include <vector>

namespace Nova {

    void AgentManager::Ask(const std::string& userPrompt, std::function<void(const std::string&)> callback, const std::string& roleOverride) {
        if (!m_Provider) {
            callback("Error: No AI Provider configured.");
            return;
        }

        std::vector<AgentMessage> messages;
        std::string activeRole = roleOverride.empty() ? m_ActiveRole : roleOverride;
        
        auto const& roles = AgentRoleRegistry::GetRoles();
        if (roles.count(activeRole)) {
            messages.push_back({"system", roles.at(activeRole).SystemPrompt});
        }
        messages.push_back({"user", userPrompt});

        auto wrappedCallback = [this, callback](const std::string& response) {
            this->ProcessCommand(response, callback);
            callback(response);
        };

        std::thread([this, messages, wrappedCallback]() {
            m_Provider->SendRequest(messages, wrappedCallback);
        }).detach();
    }

    void AgentManager::ProcessCommand(const std::string& response, std::function<void(const std::string&)> originalCallback) {
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
            else if (cmd.find("SUB_AGENT") == 0) {
                std::string role = GetParam(cmd, 0);
                std::string prompt = GetParam(cmd, 1);
                
                if (!role.empty() && !prompt.empty()) {
                    NOVA_LOG_INFO("Agentic Mode: Triggering Sub-Agent [{}] with prompt: '{}'", role, prompt);
                    
                    // Call Ask recursively with role override
                    this->Ask(prompt, [originalCallback, role](const std::string& subResponse) {
                        std::string prefix = "\n[SUB_AGENT (" + role + ") RESPONSE]:\n";
                        originalCallback(prefix + subResponse);
                    }, role);
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
            else if (cmd.find("MESHY_SPAWN") == 0) {
                std::string prompt = GetParam(cmd, 0);
                std::string name = GetParam(cmd, 1);

                if (m_MeshyProvider && !prompt.empty()) {
                    NOVA_LOG_INFO("Agentic Mode: Starting 3D Generation for '{}' via MESHY...", name);
                    
                    std::vector<AgentMessage> genMsgs;
                    genMsgs.push_back({"user", prompt});
                    
                    m_MeshyProvider->SendRequest(genMsgs, [originalCallback, name](const std::string& res) {
                        originalCallback("\n[MESHY 3D STATUS]: " + res);
                    });

                    // Spawn placeholder until mesh is ready
                    m_Scene->AddEntity(name + " (Pending Meshy)", EntityType::Mesh);
                } else {
                    originalCallback("\nError: Meshy AI Provider not configured.");
                }
            }
            else if (cmd.find("TRIPO_SPAWN") == 0) {
                std::string prompt = GetParam(cmd, 0);
                std::string name = GetParam(cmd, 1);

                if (m_TripoProvider && !prompt.empty()) {
                    NOVA_LOG_INFO("Agentic Mode: Starting 3D Generation for '{}' via TRIPO...", name);
                    
                    std::vector<AgentMessage> genMsgs;
                    genMsgs.push_back({"user", prompt});
                    
                    m_TripoProvider->SendRequest(genMsgs, [originalCallback, name](const std::string& res) {
                        originalCallback("\n[TRIPO 3D STATUS]: " + res);
                    });

                    // Spawn placeholder until mesh is ready
                    m_Scene->AddEntity(name + " (Pending Tripo)", EntityType::Mesh);
                } else {
                    originalCallback("\nError: Tripo AI Provider not configured.");
                }
            }
            else if (cmd.find("HF_SPAWN") == 0) {
                std::string prompt = GetParam(cmd, 0);
                std::string name = GetParam(cmd, 1);

                if (m_HFProvider && !prompt.empty()) {
                    NOVA_LOG_INFO("Agentic Mode: Starting 3D Generation for '{}' via Hugging Face/Open-Source...", name);
                    
                    std::vector<AgentMessage> genMsgs;
                    genMsgs.push_back({"user", prompt});
                    
                    m_HFProvider->SendRequest(genMsgs, [originalCallback, name](const std::string& res) {
                        originalCallback("\n[HF (Open Source) 3D STATUS]: " + res);
                    });

                    // Spawn placeholder until mesh is ready
                    m_Scene->AddEntity(name + " (Pending HF)", EntityType::Mesh);
                } else {
                    originalCallback("\nError: Hugging Face/Open Source Provider not configured.");
                }
            }
        }
    }

} // namespace Nova
