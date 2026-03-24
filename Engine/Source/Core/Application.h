#pragma once

#include "Core/Window.h"
#include "Core/Timer.h"
#include "Renderer/Renderer.h"
#include "Events/Event.h"

#include <memory>
#include <string>

namespace Nova {

    class Application {
    public:
        Application(const std::string& name = "Nova Engine");
        virtual ~Application();

        // Non-copyable
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        // Runs the main loop
        void Run();

        // Override these in your game/editor
        virtual void OnInit()               {}
        virtual void OnUpdate(float dt)     {}
        virtual void OnRender()             {}
        virtual void OnImGuiRender()        {}
        virtual void OnShutdown()           {}
        virtual void OnEvent(Event& event)  {}

        // Accessors
        Window&   GetWindow()   { return *m_Window; }
        Renderer& GetRenderer() { return *m_Renderer; }
        Timer&    GetTimer()    { return m_Timer; }

        static Application& Get() { return *s_Instance; }

    private:
        void OnEventInternal(Event& event);

        std::unique_ptr<Window>   m_Window;
        std::unique_ptr<Renderer> m_Renderer;
        Timer  m_Timer;
        bool   m_Running = true;

        static Application* s_Instance;
    };

    // Factory function — defined by the client application
    Application* CreateApplication();

} // namespace Nova
