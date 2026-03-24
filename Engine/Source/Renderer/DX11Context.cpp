#include "Renderer/DX11Context.h"
#include "Core/Window.h"
#include "Core/Logger.h"

#include <d3d11.h>
#include <dxgi.h>

namespace Nova {

    DX11Context::~DX11Context() {
        Shutdown();
    }

    bool DX11Context::Init(Window& window) {
        HWND hwnd = static_cast<HWND>(window.GetNativeHandle());
        if (!hwnd) {
            NOVA_ENGINE_ERROR("DX11Context::Init — Invalid HWND");
            return false;
        }

        // -- Swap chain description ------------------------------------------
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.BufferCount        = 2;
        swapChainDesc.BufferDesc.Width   = window.GetWidth();
        swapChainDesc.BufferDesc.Height  = window.GetHeight();
        swapChainDesc.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow       = hwnd;
        swapChainDesc.SampleDesc.Count   = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Windowed           = TRUE;
        swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // -- Feature levels ---------------------------------------------------
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        UINT createFlags = 0;
#ifdef _DEBUG
        createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL achievedLevel;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,                    // Adapter (default)
            D3D_DRIVER_TYPE_HARDWARE,   // Hardware acceleration
            nullptr,                    // Software rasterizer (none)
            createFlags,
            featureLevels,
            _countof(featureLevels),
            D3D11_SDK_VERSION,
            &swapChainDesc,
            m_SwapChain.GetAddressOf(),
            m_Device.GetAddressOf(),
            &achievedLevel,
            m_DeviceContext.GetAddressOf()
        );

        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("D3D11CreateDeviceAndSwapChain failed: 0x{:08X}", static_cast<unsigned>(hr));
            return false;
        }

        NOVA_ENGINE_INFO("DirectX 11 initialized — Feature Level {}.{}",
            (achievedLevel >> 12) & 0xF,
            (achievedLevel >> 8)  & 0xF
        );

        // -- Create render target & depth stencil ----------------------------
        if (!CreateRenderTarget()) return false;
        if (!CreateDepthStencil(window.GetWidth(), window.GetHeight())) return false;

        // -- Set viewport ----------------------------------------------------
        m_Viewport.TopLeftX = 0.0f;
        m_Viewport.TopLeftY = 0.0f;
        m_Viewport.Width    = static_cast<float>(window.GetWidth());
        m_Viewport.Height   = static_cast<float>(window.GetHeight());
        m_Viewport.MinDepth = 0.0f;
        m_Viewport.MaxDepth = 1.0f;
        m_DeviceContext->RSSetViewports(1, &m_Viewport);

        // -- Rasterizer state (no backface culling) ---------------------------
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.DepthClipEnable = TRUE;

        hr = m_Device->CreateRasterizerState(&rasterDesc, m_RasterState.GetAddressOf());
        if (SUCCEEDED(hr)) {
            m_DeviceContext->RSSetState(m_RasterState.Get());
        }

        return true;
    }

    void DX11Context::Shutdown() {
        m_RasterState.Reset();
        m_DepthStencilView.Reset();
        m_DepthStencilBuffer.Reset();
        m_RenderTargetView.Reset();
        m_SwapChain.Reset();
        m_DeviceContext.Reset();
        m_Device.Reset();
        NOVA_ENGINE_INFO("DirectX 11 shutdown");
    }

    void DX11Context::BindDefaultState() {
        if (m_DeviceContext) {
            m_DeviceContext->RSSetViewports(1, &m_Viewport);
            m_DeviceContext->RSSetState(m_RasterState.Get());
        }
    }

    void DX11Context::ResizeBuffers(int width, int height) {
        if (width == 0 || height == 0) return;

        m_RenderTargetView.Reset();
        m_DepthStencilView.Reset();
        m_DepthStencilBuffer.Reset();

        HRESULT hr = m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("SwapChain::ResizeBuffers failed: 0x{:08X}", static_cast<unsigned>(hr));
            return;
        }

        CreateRenderTarget();
        CreateDepthStencil(width, height);

        m_Viewport.Width    = static_cast<float>(width);
        m_Viewport.Height   = static_cast<float>(height);
        m_Viewport.MinDepth = 0.0f;
        m_Viewport.MaxDepth = 1.0f;
        m_DeviceContext->RSSetViewports(1, &m_Viewport);
    }

    bool DX11Context::CreateRenderTarget() {
        ComPtr<ID3D11Texture2D> backBuffer;
        HRESULT hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("GetBuffer failed: 0x{:08X}", static_cast<unsigned>(hr));
            return false;
        }

        hr = m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.GetAddressOf());
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("CreateRenderTargetView failed: 0x{:08X}", static_cast<unsigned>(hr));
            return false;
        }

        return true;
    }

    bool DX11Context::CreateDepthStencil(int width, int height) {
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width              = width;
        depthDesc.Height             = height;
        depthDesc.MipLevels          = 1;
        depthDesc.ArraySize          = 1;
        depthDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count   = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage              = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;

        HRESULT hr = m_Device->CreateTexture2D(&depthDesc, nullptr, m_DepthStencilBuffer.GetAddressOf());
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("CreateTexture2D (depth) failed: 0x{:08X}", static_cast<unsigned>(hr));
            return false;
        }

        hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, m_DepthStencilView.GetAddressOf());
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("CreateDepthStencilView failed: 0x{:08X}", static_cast<unsigned>(hr));
            return false;
        }

        // Bind render target + depth stencil
        m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

        return true;
    }

} // namespace Nova
