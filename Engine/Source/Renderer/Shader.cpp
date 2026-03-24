#include "Renderer/Shader.h"
#include "Core/Logger.h"

namespace Nova {

    bool Shader::CompileFromSource(
        ID3D11Device* device,
        const std::string& vertexSrc,
        const std::string& pixelSrc)
    {
        HRESULT hr;
        ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;

        // ── Compile Vertex Shader ───────────────────────────────────────────
        hr = D3DCompile(
            vertexSrc.c_str(), vertexSrc.size(),
            "VertexShader", nullptr, nullptr,
            "VSMain", "vs_5_0",
            D3DCOMPILE_ENABLE_STRICTNESS, 0,
            vsBlob.GetAddressOf(), errorBlob.GetAddressOf()
        );

        if (FAILED(hr)) {
            if (errorBlob) {
                NOVA_ENGINE_ERROR("VS compile error: {}",
                    static_cast<const char*>(errorBlob->GetBufferPointer()));
            }
            return false;
        }

        hr = device->CreateVertexShader(
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            nullptr, m_VertexShader.GetAddressOf()
        );
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("CreateVertexShader failed: 0x{:08X}", (unsigned)hr);
            return false;
        }

        // ── Compile Pixel Shader ────────────────────────────────────────────
        errorBlob.Reset();
        hr = D3DCompile(
            pixelSrc.c_str(), pixelSrc.size(),
            "PixelShader", nullptr, nullptr,
            "PSMain", "ps_5_0",
            D3DCOMPILE_ENABLE_STRICTNESS, 0,
            psBlob.GetAddressOf(), errorBlob.GetAddressOf()
        );

        if (FAILED(hr)) {
            if (errorBlob) {
                NOVA_ENGINE_ERROR("PS compile error: {}",
                    static_cast<const char*>(errorBlob->GetBufferPointer()));
            }
            return false;
        }

        hr = device->CreatePixelShader(
            psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
            nullptr, m_PixelShader.GetAddressOf()
        );
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("CreatePixelShader failed: 0x{:08X}", (unsigned)hr);
            return false;
        }

        // ── Input Layout (Position + Color + Normal) ────────────────────────
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        hr = device->CreateInputLayout(
            layout, _countof(layout),
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            m_InputLayout.GetAddressOf()
        );
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("CreateInputLayout failed: 0x{:08X}", (unsigned)hr);
            return false;
        }

        NOVA_ENGINE_INFO("Shaders compiled successfully");
        return true;
    }

    void Shader::Bind(ID3D11DeviceContext* ctx) const {
        ctx->IASetInputLayout(m_InputLayout.Get());
        ctx->VSSetShader(m_VertexShader.Get(), nullptr, 0);
        ctx->PSSetShader(m_PixelShader.Get(), nullptr, 0);
    }

} // namespace Nova
