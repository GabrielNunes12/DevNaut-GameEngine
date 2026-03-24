#include "Renderer/Buffer.h"
#include "Core/Logger.h"

namespace Nova {

    // ── VertexBuffer ────────────────────────────────────────────────────────

    bool VertexBuffer::Create(ID3D11Device* device, const Vertex* vertices, uint32_t count) {
        m_Count = count;

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth      = sizeof(Vertex) * count;
        desc.Usage           = D3D11_USAGE_DEFAULT;
        desc.BindFlags       = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = vertices;

        HRESULT hr = device->CreateBuffer(&desc, &data, m_Buffer.GetAddressOf());
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("VertexBuffer::Create failed: 0x{:08X}", (unsigned)hr);
            return false;
        }
        return true;
    }

    void VertexBuffer::Bind(ID3D11DeviceContext* ctx) const {
        UINT offset = 0;
        ctx->IASetVertexBuffers(0, 1, m_Buffer.GetAddressOf(), &m_Stride, &offset);
    }

    // ── IndexBuffer ─────────────────────────────────────────────────────────

    bool IndexBuffer::Create(ID3D11Device* device, const uint32_t* indices, uint32_t count) {
        m_Count = count;

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth      = sizeof(uint32_t) * count;
        desc.Usage           = D3D11_USAGE_DEFAULT;
        desc.BindFlags       = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = indices;

        HRESULT hr = device->CreateBuffer(&desc, &data, m_Buffer.GetAddressOf());
        if (FAILED(hr)) {
            NOVA_ENGINE_ERROR("IndexBuffer::Create failed: 0x{:08X}", (unsigned)hr);
            return false;
        }
        return true;
    }

    void IndexBuffer::Bind(ID3D11DeviceContext* ctx) const {
        ctx->IASetIndexBuffer(m_Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    }

} // namespace Nova
