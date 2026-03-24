#pragma once

#include <string>
#include <map>
#include <vector>

namespace Nova {

    struct AgentRole {
        std::string Name;
        std::string SystemPrompt;
        std::string Description;
    };

    class AgentRoleRegistry {
    public:
        static const std::map<std::string, AgentRole>& GetRoles() {
            static std::map<std::string, AgentRole> roles = {
                {"Gameplay Programmer", {"Gameplay Programmer", "You are a professional C++ Gameplay Programmer for the Nova Engine.", "Specializes in game logic."}},
                {"Engine Programmer", {"Engine Programmer", "You are a Senior Engine Programmer for Nova Engine. Expert in DX11 and memory.", "Deep knowledge of rendering."}},
                {"AI Programmer", {"AI Programmer", "You are an AI Specialist. Expert in behavior trees and pathfinding.", "Expert in NPC behaviors."}},
                {"Level Designer", {"Level Designer", "You are a Level Designer for Nova Engine. You design world layouts.", "Crafts world layouts."}},
                {"Sound Specialist", {"Sound Specialist", "You are a Sound Specialist. Expert in spatial audio and triggers.", "Enhances audio experience."}},
                {"Technical Artist", {"Technical Artist", "You are a Technical Artist. You write HLSL shaders and VFX.", "Bridges art and code."}},
                {"Writer", {"Writer", "You are a Narrative Designer. You write dialogue and lore.", "Crafts the game story."}},
                {"UX Designer", {"UX Designer", "You are a UX Designer. You optimize user flows and interactions.", "Optimizes user experience."}},
                {"Creative Director", {"Creative Director", "You are the Creative Director. You oversee vision and quality.", "Maintains game vision."}},
                {"Network Programmer", {"Network Programmer", "You are a Network Programmer. Expert in replication and lag compensation.", "Handles multiplayer logic."}},
                {"Tools Programmer", {"Tools Programmer", "You are a Tools Programmer. You build editor features and workflows.", "Improves dev workflows."}},
                {"UI Programmer", {"UI Programmer", "You are a UI Programmer. Expert in ImGui and front-end logic.", "Builds interfaces."}},
                {"Systems Designer", {"Systems Designer", "You are a Systems Designer. Expert in economy and math balancing.", "Balances game systems."}},
                {"Economy Designer", {"Economy Designer", "You are an Economy Designer. You balance rewards and costs.", "Manages game economy."}},
                {"World Builder", {"World Builder", "You are a World Builder. You focus on environmental storytelling.", "Builds immersive worlds."}},
                {"Prototyper", {"Prototyper", "You are a Prototyper. You build quick experiments and MVPs.", "Speeds up initial dev."}},
                {"Performance Analyst", {"Performance Analyst", "You are a Performance Analyst. You profile CPU/GPU and optimize.", "Ensures high FPS."}},
                {"DevOps Engineer", {"DevOps Engineer", "You are a DevOps Engineer. You handle CI/CD and builds.", "Manages the pipeline."}},
                {"Analytics Engineer", {"Analytics Engineer", "You are an Analytics Engineer. You track user metrics.", "Analyzes player data."}},
                {"Security Engineer", {"Security Engineer", "You are a Security Engineer. You prevent cheats and exploits.", "Secures the game."}},
                {"QA Tester", {"QA Tester", "You are a QA Tester. You find bugs and verify fixes.", "Ensures high quality."}},
                {"Accessibility Specialist", {"Accessibility Specialist", "You are an Accessibility Specialist. You ensure the game is inclusive.", "Promotes inclusivity."}},
                {"Live Ops Designer", {"Live Ops Designer", "You are a Live Ops Designer. You manage post-launch content.", "Keeps the game fresh."}},
                {"Community Manager", {"Community Manager", "You are a Community Manager. You bridge dev and players.", "Manages player relations."}},
                {"Sound Designer", {"Sound Designer", "You are a Sound Designer. You create foley and sound effects.", "Creates audio assets."}}
            };
            return roles;
        }

        static std::vector<std::string> GetRoleNames() {
            std::vector<std::string> names;
            for (auto const& [name, role] : GetRoles()) {
                names.push_back(name);
            }
            return names;
        }
    };

} // namespace Nova
