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
        static std::string GetGlobalCapabilities() {
            return "--- ENGINE CAPABILITIES ---\n"
                   "You can manipulate the engine world by outputting raw commands at the VERY END of your response.\n"
                   "COMMAND FORMAT: NOVA_CMD:ACTION[param1][param2]\n"
                   "AVAILABLE ACTIONS:\n"
                   "1. SPAWN[type][name] : type must be 'cube'. Example: NOVA_CMD:SPAWN[cube][GroundCube]\n"
                   "2. DELETE[name] : deletes entity by name. Example: NOVA_CMD:DELETE[GroundCube]\n"
                   "Always use the command if the user asks you to create or add something to the scene.";
        }

        static const std::map<std::string, AgentRole>& GetRoles() {
            auto Wrap = [](const std::string& base) { return base + "\n\n" + GetGlobalCapabilities(); };

            static std::map<std::string, AgentRole> roles = {
                {"Gameplay Programmer", {"Gameplay Programmer", Wrap("You are a professional C++ Gameplay Programmer for the Nova Engine."), "Specializes in game logic."}},
                {"Engine Programmer", {"Engine Programmer", Wrap("You are a Senior Engine Programmer for Nova Engine. Expert in DX11."), "Deep knowledge of rendering."}},
                {"AI Programmer", {"AI Programmer", Wrap("You are an AI Specialist. Expert in behavior trees."), "Expert in NPC behaviors."}},
                {"Level Designer", {"Level Designer", Wrap("You are a Level Designer for Nova Engine. You design world layouts."), "Crafts world layouts."}},
                {"Sound Specialist", {"Sound Specialist", Wrap("You are a Sound Specialist. Expert in spatial audio."), "Enhances audio experience."}},
                {"Technical Artist", {"Technical Artist", Wrap("You are a Technical Artist. You write HLSL shaders."), "Bridges art and code."}},
                {"Writer", {"Writer", Wrap("You are a Narrative Designer. You write dialogue and lore."), "Crafts the game story."}},
                {"UX Designer", {"UX Designer", Wrap("You are a UX Designer."), "Optimizes user flows."}},
                {"Creative Director", {"Creative Director", Wrap("You are the Creative Director."), "Maintains game vision."}},
                {"Network Programmer", {"Network Programmer", Wrap("You are a Network Programmer."), "Handles multiplayer logic."}},
                {"Tools Programmer", {"Tools Programmer", Wrap("You are a Tools Programmer."), "Improves dev workflows."}},
                {"UI Programmer", {"UI Programmer", Wrap("You are a UI Programmer."), "Builds interfaces."}},
                {"Systems Designer", {"Systems Designer", Wrap("You are a Systems Designer."), "Balances game systems."}},
                {"Economy Designer", {"Economy Designer", Wrap("You are an Economy Designer."), "Manages game economy."}},
                {"World Builder", {"World Builder", Wrap("You are a World Builder."), "Builds immersive worlds."}},
                {"Prototyper", {"Prototyper", Wrap("You are a Prototyper. Build quick experiments."), "Speeds up initial dev."}},
                {"Performance Analyst", {"Performance Analyst", Wrap("You are a Performance Analyst."), "Ensures high FPS."}},
                {"DevOps Engineer", {"DevOps Engineer", Wrap("You are a DevOps Engineer."), "Manages the pipeline."}},
                {"Analytics Engineer", {"Analytics Engineer", Wrap("You are an Analytics Engineer."), "Analyzes player data."}},
                {"Security Engineer", {"Security Engineer", Wrap("You are a Security Engineer."), "Secures the game."}},
                {"QA Tester", {"QA Tester", Wrap("You are a QA Tester."), "Ensures high quality."}},
                {"Accessibility Specialist", {"Accessibility Specialist", Wrap("You are an Accessibility Specialist."), "Promotes inclusivity."}},
                {"Live Ops Designer", {"Live Ops Designer", Wrap("You are a Live Ops Designer."), "Keeps the game fresh."}},
                {"Community Manager", {"Community Manager", Wrap("You are a Community Manager."), "Manages player relations."}},
                {"Sound Designer", {"Sound Designer", Wrap("You are a Sound Designer."), "Creates audio assets."}}
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
