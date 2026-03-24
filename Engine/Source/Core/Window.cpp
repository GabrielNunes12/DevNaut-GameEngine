#include "Core/Window.h"
#include "Core/Logger.h"
#include "Events/Event.h"
#include "Input/InputManager.h"
#include "UI/ImGuiLayer.h"

#include <SDL.h>
#include <SDL_syswm.h>

namespace Nova {

    Window::Window(const WindowProps& props)
        : m_Title(props.Title), m_Width(props.Width), m_Height(props.Height)
    {
        // Initialize SDL video subsystem
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            NOVA_ENGINE_ERROR("Failed to initialize SDL: {}", SDL_GetError());
            return;
        }

        m_Window = SDL_CreateWindow(
            m_Title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_Width,
            m_Height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );

        if (!m_Window) {
            NOVA_ENGINE_ERROR("Failed to create SDL window: {}", SDL_GetError());
            return;
        }

        NOVA_ENGINE_INFO("Window created: {}x{} - \"{}\"", m_Width, m_Height, m_Title);
    }

    Window::~Window() {
        if (m_Window) {
            SDL_DestroyWindow(m_Window);
            m_Window = nullptr;
        }
        SDL_Quit();
        NOVA_ENGINE_INFO("Window destroyed");
    }

    void Window::PollEvents() {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            // Forward to ImGui
            ImGuiLayer::ProcessEvent(sdlEvent);

            // Forward to InputManager for scroll/event accumulation
            InputManager::ProcessEvent(sdlEvent);

            switch (sdlEvent.type) {
                case SDL_QUIT: {
                    m_ShouldClose = true;
                    if (m_EventCallback) {
                        Event e(EventType::WindowClose);
                        m_EventCallback(e);
                    }
                    break;
                }
                case SDL_WINDOWEVENT: {
                    if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
                        m_Width  = sdlEvent.window.data1;
                        m_Height = sdlEvent.window.data2;
                        if (m_EventCallback) {
                            Event e(EventType::WindowResize);
                            m_EventCallback(e);
                        }
                    }
                    break;
                }
                case SDL_KEYDOWN: {
                    if (m_EventCallback) {
                        Event e(EventType::KeyPressed);
                        m_EventCallback(e);
                    }
                    break;
                }
                case SDL_KEYUP: {
                    if (m_EventCallback) {
                        Event e(EventType::KeyReleased);
                        m_EventCallback(e);
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    if (m_EventCallback) {
                        Event e(EventType::MouseMoved);
                        m_EventCallback(e);
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if (m_EventCallback) {
                        Event e(EventType::MouseButtonPressed);
                        m_EventCallback(e);
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    if (m_EventCallback) {
                        Event e(EventType::MouseButtonReleased);
                        m_EventCallback(e);
                    }
                    break;
                }
            }
        }
    }

    void* Window::GetNativeHandle() const {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        if (SDL_GetWindowWMInfo(m_Window, &wmInfo)) {
            return wmInfo.info.win.window;
        }
        NOVA_ENGINE_ERROR("Failed to get native window handle");
        return nullptr;
    }

} // namespace Nova
