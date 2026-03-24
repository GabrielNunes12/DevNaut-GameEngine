#pragma once

#include <imgui.h>
#include "AI/AgentManager.h"
#include "AI/GeminiProvider.h"
#include "AI/OpenAIProvider.h"
#include "AI/ClaudeProvider.h"
#include "AI/LocalProvider.h"
#include <string>
#include <vector>

namespace Nova {

    class AgentInspector {
    public:
        void OnImGuiRender() {
            ImGui::Begin("AI Agent Command Center");

            // --- Configuration ---
            if (ImGui::CollapsingHeader("AI Providers", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginTabBar("ProviderTabs")) {
                    // --- Gemini ---
                    if (ImGui::BeginTabItem("Gemini")) {
                        static char apiKey[128] = "";
                        static char model[64] = "gemini-1.5-flash";
                        ImGui::InputText("API Key##Gemini", apiKey, sizeof(apiKey), ImGuiInputTextFlags_Password);
                        ImGui::InputText("Model##Gemini", model, sizeof(model));
                        if (ImGui::Button("Connect Gemini")) {
                            auto provider = std::make_shared<GeminiProvider>(apiKey);
                            provider->SetModel(model);
                            AgentManager::Instance().SetProvider(provider);
                            m_StatusMessage = "Gemini Connected.";
                        }
                        ImGui::EndTabItem();
                    }
                    // --- OpenAI ---
                    if (ImGui::BeginTabItem("OpenAI")) {
                        static char apiKey[128] = "";
                        static char model[64] = "gpt-4o";
                        ImGui::InputText("API Key##OpenAI", apiKey, sizeof(apiKey), ImGuiInputTextFlags_Password);
                        ImGui::InputText("Model##OpenAI", model, sizeof(model));
                        if (ImGui::Button("Connect OpenAI")) {
                            auto provider = std::make_shared<OpenAIProvider>(apiKey);
                            provider->SetModel(model);
                            AgentManager::Instance().SetProvider(provider);
                            m_StatusMessage = "OpenAI Connected.";
                        }
                        ImGui::EndTabItem();
                    }
                    // --- Claude ---
                    if (ImGui::BeginTabItem("Claude")) {
                        static char apiKey[128] = "";
                        static char model[64] = "claude-3-5-sonnet-20240620";
                        ImGui::InputText("API Key##Claude", apiKey, sizeof(apiKey), ImGuiInputTextFlags_Password);
                        ImGui::InputText("Model##Claude", model, sizeof(model));
                        if (ImGui::Button("Connect Claude")) {
                            auto provider = std::make_shared<ClaudeProvider>(apiKey);
                            provider->SetModel(model);
                            AgentManager::Instance().SetProvider(provider);
                            m_StatusMessage = "Claude Connected.";
                        }
                        ImGui::EndTabItem();
                    }
                    // --- Local ---
                    if (ImGui::BeginTabItem("Local")) {
                        static char endpoint[256] = "http://localhost:11434/v1/chat/completions";
                        static char model[64] = "llama3";
                        ImGui::InputText("Endpoint", endpoint, sizeof(endpoint));
                        ImGui::InputText("Model##Local", model, sizeof(model));
                        if (ImGui::Button("Connect Local")) {
                            auto provider = std::make_shared<LocalProvider>(endpoint);
                            provider->SetModel(model);
                            AgentManager::Instance().SetProvider(provider);
                            m_StatusMessage = "Local Provider Connected.";
                        }
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Active: %s | %s", 
                    AgentManager::Instance().GetProvider() ? AgentManager::Instance().GetProvider()->GetName().c_str() : "None",
                    m_StatusMessage.c_str());
            }

            ImGui::Separator();

            // --- Role Selection ---
            static int currentRole = 0;
            auto roles = AgentRoleRegistry::GetRoleNames();
            std::vector<const char*> rolePtrs;
            for (auto const& r : roles) rolePtrs.push_back(r.c_str());

            if (ImGui::Combo("Agent Role", &currentRole, rolePtrs.data(), (int)rolePtrs.size())) {
                AgentManager::Instance().SetActiveRole(roles[currentRole]);
            }

            // --- Chat ---
            ImGui::BeginChild("ChatLog", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);
            for (auto const& entry : m_ChatEntries) {
                ImGui::TextWrapped("[%s]: %s", entry.first.c_str(), entry.second.c_str());
                ImGui::Spacing();
            }
            if (m_WaitingForAI) {
                ImGui::TextDisabled("AI is thinking...");
            }
            ImGui::EndChild();

            static char prompt[256] = "";
            bool reclaim_focus = false;
            if (ImGui::InputText("Prompt", prompt, sizeof(prompt), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string p = prompt;
                if (!p.empty()) {
                    m_ChatEntries.push_back({"You", p});
                    m_WaitingForAI = true;
                    AgentManager::Instance().Ask(p, [this](const std::string& response) {
                        m_ChatEntries.push_back({"AI", response});
                        m_WaitingForAI = false;
                    });
                    prompt[0] = '\0';
                    reclaim_focus = true;
                }
            }
            if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1);

            ImGui::End();
        }

    private:
        std::vector<std::pair<std::string, std::string>> m_ChatEntries;
        std::string m_StatusMessage = "Idle";
        bool m_WaitingForAI = false;
    };

} // namespace Nova
