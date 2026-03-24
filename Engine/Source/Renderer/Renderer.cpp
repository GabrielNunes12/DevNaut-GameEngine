#include "Renderer/Renderer.h"
#include "Core/Window.h"
#include "Core/Logger.h"

namespace Nova {

    bool Renderer::Init(Window& window) {
        if (!m_Context.Init(window)) {
            NOVA_ENGINE_ERROR("Renderer::Init — DX11 context initialization failed");
            return false;
        }
        NOVA_ENGINE_INFO("Renderer initialized");
        return true;
    }

    void Renderer::Shutdown() {
        m_Context.Shutdown();
    }

    void Renderer::BeginFrame() {
        auto* ctx = m_Context.GetDeviceContext();
        auto* rtv = m_Context.GetRenderTarget();
        auto* dsv = m_Context.GetDepthStencil();

        // Re-bind render targets (FLIP_DISCARD unbinds them after Present)
        ctx->OMSetRenderTargets(1, &rtv, dsv);

        // ImGui Multi-Viewports can modify the rasterizer state and viewport,
        // so we must ensure our default state is bound before drawing the scene.
        m_Context.BindDefaultState();

        ctx->ClearRenderTargetView(rtv, m_ClearColor.data());
        ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void Renderer::EndFrame() {
        // Present with VSync (1 = on, 0 = off)
        m_Context.GetSwapChain()->Present(1, 0);
    }

    void Renderer::OnResize(int width, int height) {
        m_Context.ResizeBuffers(width, height);
    }

    void Renderer::SetClearColor(float r, float g, float b, float a) {
        m_ClearColor = { r, g, b, a };
    }

} // namespace Nova
