#pragma once
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace Nova {

    // Strategy interface for editor operations
    class EditorAction {
    public:
        virtual ~EditorAction() = default;
        virtual const char* GetLabel() const = 0;
        virtual void Execute() = 0;
    };

    // Generic lambda-based action for quick registration
    class LambdaAction : public EditorAction {
    public:
        LambdaAction(const std::string& label, std::function<void()> func)
            : m_Label(label), m_Func(func) {}

        const char* GetLabel() const override { return m_Label.c_str(); }
        void Execute() override { m_Func(); }

    private:
        std::string m_Label;
        std::function<void()> m_Func;
    };

    // Helper to group actions under a menu name
    struct MenuCategory {
        std::string Name;
        std::vector<std::shared_ptr<EditorAction>> Actions;
    };

} // namespace Nova
