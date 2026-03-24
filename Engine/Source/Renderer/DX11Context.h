#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi.h>

namespace Nova {

    class Window;

    class DX11Context {
    public:
        DX11Context() = default;
        ~DX11Context();

        // Non-copyable
        DX11Context(const DX11Context&) = delete;
        DX11Context& operator=(const DX11Context&) = delete;

        bool Init(Window& window);
        void Shutdown();

        void ResizeBuffers(int width, int height);

        // Call every frame to ensure ImGui hasn't nuked our state
        void BindDefaultState();

        // Accessors
        ID3D11Device*           GetDevice()        const { return m_Device.Get(); }
        ID3D11DeviceContext*    GetDeviceContext()  const { return m_DeviceContext.Get(); }
        IDXGISwapChain*         GetSwapChain()      const { return m_SwapChain.Get(); }
        ID3D11RenderTargetView* GetRenderTarget()  const { return m_RenderTargetView.Get(); }
        ID3D11DepthStencilView* GetDepthStencil()  const { return m_DepthStencilView.Get(); }

    private:
        bool CreateRenderTarget();
        bool CreateDepthStencil(int width, int height);

        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

        ComPtr<ID3D11Device>            m_Device;
        ComPtr<ID3D11DeviceContext>     m_DeviceContext;
        ComPtr<IDXGISwapChain>          m_SwapChain;
        ComPtr<ID3D11RenderTargetView>  m_RenderTargetView;
        ComPtr<ID3D11Texture2D>         m_DepthStencilBuffer;
        ComPtr<ID3D11DepthStencilView>  m_DepthStencilView;

        ComPtr<ID3D11RasterizerState>   m_RasterState;
        D3D11_VIEWPORT                  m_Viewport = {};
    };

} // namespace Nova
