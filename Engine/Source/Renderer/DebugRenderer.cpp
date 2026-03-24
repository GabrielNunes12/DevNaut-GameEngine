#include "Renderer/DebugRenderer.h"
#include "Core/Logger.h"

namespace Nova {

    static const char* g_DebugShaderSrc = R"(
    cbuffer MVPBuffer : register(b0) {
        matrix ViewProjection;
    };

    struct VSInput {
        float3 Position : POSITION;
        float4 Color    : COLOR;
    };

    struct PSInput {
        float4 Position : SV_POSITION;
        float4 Color    : COLOR;
    };

    PSInput VSMain(VSInput input) {
        PSInput output;
        output.Position = mul(float4(input.Position, 1.0f), ViewProjection);
        output.Color = input.Color;
        return output;
    }

    float4 PSMain(PSInput input) : SV_TARGET {
        return input.Color;
    }
    )";

    DebugRenderer::DebugRenderer() {}

    bool DebugRenderer::Init(ID3D11Device* device) {
        m_Shader = std::make_unique<Shader>();
        if (!m_Shader->CompileFromSource(device, g_DebugShaderSrc, g_DebugShaderSrc)) {
            return false;
        }

        // Create Dynamic Vertex Buffer
        D3D11_BUFFER_DESC vbd = {};
        vbd.Usage = D3D11_USAGE_DYNAMIC;
        vbd.ByteWidth = sizeof(DebugVertex) * m_MaxVertices;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = device->CreateBuffer(&vbd, nullptr, m_VertexBuffer.GetAddressOf());
        if (FAILED(hr)) return false;

        // Create Constant Buffer for ViewProjection
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.ByteWidth = sizeof(DirectX::XMMATRIX);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = device->CreateBuffer(&cbd, nullptr, m_ConstantBuffer.GetAddressOf());
        return SUCCEEDED(hr);
    }

    void DebugRenderer::DrawLine(const Vec3& start, const Vec3& end, const Vec4& color) {
        if (m_Vertices.size() + 2 > m_MaxVertices) return;
        m_Vertices.push_back({ start, color });
        m_Vertices.push_back({ end, color });
    }

    void DebugRenderer::DrawLine(const Vec3& start, const Vec3& end, uint32_t colorRGBA) {
        // RP3D uses 0xAABBGGRR or similar, let's convert to float4
        float r = ((colorRGBA >> 0) & 0xFF) / 255.0f;
        float g = ((colorRGBA >> 8) & 0xFF) / 255.0f;
        float b = ((colorRGBA >> 16) & 0xFF) / 255.0f;
        float a = ((colorRGBA >> 24) & 0xFF) / 255.0f;
        DrawLine(start, end, { r, g, b, a });
    }

    void DebugRenderer::Flush(ID3D11DeviceContext* ctx, const DirectX::XMMATRIX& viewProjection) {
        if (m_Vertices.empty()) return;

        // Update constant buffer
        D3D11_MAPPED_SUBRESOURCE mappedCB;
        if (SUCCEEDED(ctx->Map(m_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCB))) {
            DirectX::XMMATRIX vpTransposed = DirectX::XMMatrixTranspose(viewProjection);
            memcpy(mappedCB.pData, &vpTransposed, sizeof(vpTransposed));
            ctx->Unmap(m_ConstantBuffer.Get(), 0);
        }

        // Update vertex buffer
        D3D11_MAPPED_SUBRESOURCE mappedVB;
        if (SUCCEEDED(ctx->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB))) {
            memcpy(mappedVB.pData, m_Vertices.data(), sizeof(DebugVertex) * m_Vertices.size());
            ctx->Unmap(m_VertexBuffer.Get(), 0);
        }

        // Bind
        m_Shader->Bind(ctx);
        ctx->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());
        
        uint32_t stride = sizeof(DebugVertex);
        uint32_t offset = 0;
        ctx->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        // Draw
        ctx->Draw((uint32_t)m_Vertices.size(), 0);

        m_Vertices.clear();
    }

} // namespace Nova
