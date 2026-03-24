#pragma once
#include "Command.h"
#include <vector>
#include <memory>

namespace Nova {

    // Command History tracking for Undo/Redo capabilities
    class CommandHistory {
    public:
        void AddCommand(std::shared_ptr<Command> cmd) {
            // Discard any redos if we diverged
            if (m_CurrentIndex < m_Commands.size()) {
                m_Commands.erase(m_Commands.begin() + m_CurrentIndex, m_Commands.end());
            }
            m_Commands.push_back(cmd);
            m_CurrentIndex++;
            // Note: We do not call Execute() here because editor continuous 
            // interactions manually apply the changes until mouse release.
        }

        void Undo() {
            if (m_CurrentIndex > 0) {
                m_CurrentIndex--;
                m_Commands[m_CurrentIndex]->Undo();
            }
        }

        void Redo() {
            if (m_CurrentIndex < m_Commands.size()) {
                m_Commands[m_CurrentIndex]->Execute();
                m_CurrentIndex++;
            }
        }

        void Clear() {
            m_Commands.clear();
            m_CurrentIndex = 0;
        }

    private:
        std::vector<std::shared_ptr<Command>> m_Commands;
        size_t m_CurrentIndex = 0;
    };

}
