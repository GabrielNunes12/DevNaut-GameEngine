#include "Core/Application.h"
#include "Core/Logger.h"
#include "Input/InputManager.h"
#include "Events/EventDispatcher.h"
#include "UI/ImGuiLayer.h"

namespace Nova {

    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name) {
        s_Instance = this;

        // Initialize logging first
        Logger::Init();
        NOVA_ENGINE_INFO("DevNautEngine v0.1.0");

        // Create window
        WindowProps props;
        props.Title  = name;
        props.Width  = 1280;
        props.Height = 720;
        m_Window = std::make_unique<Window>(props);
        m_Window->SetEventCallback([this](Event& e) { OnEventInternal(e); });

        // Create renderer
        m_Renderer = std::make_unique<Renderer>();
        if (!m_Renderer->Init(*m_Window)) {
            NOVA_ENGINE_ERROR("Failed to initialize renderer");
            m_Running = false;
            return;
        }

        NOVA_ENGINE_INFO("Engine initialized successfully");

        // Initialize input system
        InputManager::Init();

        // Initialize ImGui
        ImGuiLayer::Init(*m_Window, m_Renderer->GetContext());
    }

    Application::~Application() {
        s_Instance = nullptr;
    }

    void Application::Run() {
        OnInit();

        while (m_Running && !m_Window->ShouldClose()) {
            float dt = m_Timer.Tick();

            // Poll SDL events
            m_Window->PollEvents();

            // Update input state
            InputManager::Update();

            // User update
            OnUpdate(dt);

            // Render
            m_Renderer->BeginFrame();
            OnRender();
            // Render ImGui
            ImGuiLayer::BeginFrame();
            OnImGuiRender();
            ImGuiLayer::EndFrame();

            // Swap buffers
            m_Renderer->EndFrame();
        }

        OnShutdown();
        ImGuiLayer::Shutdown();
        m_Renderer->Shutdown();
        NOVA_ENGINE_INFO("Engine shutdown complete");
    }

    void Application::OnEventInternal(Event& event) {
        EventDispatcher dispatcher(event);

        // Handle window close
        dispatcher.Dispatch<EventType::WindowClose>([this](Event& e) -> bool {
            m_Running = false;
            return true;
        });

        // Handle window resize
        dispatcher.Dispatch<EventType::WindowResize>([this](Event& e) -> bool {
            m_Renderer->OnResize(m_Window->GetWidth(), m_Window->GetHeight());
            return false;
        });

        // Forward to user
        OnEvent(event);
    }

} // namespace Nova
