#pragma once

union SDL_Event;

namespace Nova {

    class Window;
    class DX11Context;

    class ImGuiLayer {
    public:
        static void Init(Window& window, DX11Context& context);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        // Forward SDL events to ImGui
        static void ProcessEvent(const SDL_Event& event);
    };

} // namespace Nova
