#pragma once

#include "Renderer/DX11Context.h"
#include <array>

namespace Nova {

    class Window;

    class Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        bool Init(Window& window);
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        void OnResize(int width, int height);

        void SetClearColor(float r, float g, float b, float a = 1.0f);

        DX11Context&       GetContext()       { return m_Context; }
        const DX11Context& GetContext() const { return m_Context; }

    private:
        DX11Context m_Context;
        std::array<float, 4> m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
    };

} // namespace Nova
