#pragma once

#include <string>
#include <functional>

struct SDL_Window;

namespace Nova {

    struct WindowProps {
        std::string Title = "Nova Engine";
        int Width  = 1280;
        int Height = 720;
    };

    class Window {
    public:
        using EventCallback = std::function<void(class Event&)>;

        Window(const WindowProps& props = WindowProps());
        ~Window();

        // Non-copyable
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void PollEvents();

        int  GetWidth()  const { return m_Width; }
        int  GetHeight() const { return m_Height; }
        bool ShouldClose() const { return m_ShouldClose; }
        void Close() { m_ShouldClose = true; }

        void SetEventCallback(const EventCallback& callback) { m_EventCallback = callback; }

        SDL_Window* GetSDLWindow() const { return m_Window; }
        void*       GetNativeHandle() const; // Returns HWND on Windows

    private:
        SDL_Window* m_Window = nullptr;
        std::string m_Title;
        int  m_Width  = 0;
        int  m_Height = 0;
        bool m_ShouldClose = false;
        EventCallback m_EventCallback;
    };

} // namespace Nova
