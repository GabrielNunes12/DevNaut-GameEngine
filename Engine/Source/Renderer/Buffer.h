#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <cstdint>

namespace Nova {

    // ── Vertex Layout ───────────────────────────────────────────────────────
    struct Vertex {
        float Position[3];
        float Color[4];
        float Normal[3];
    };

    // ── Vertex Buffer ───────────────────────────────────────────────────────
    class VertexBuffer {
    public:
        VertexBuffer() = default;

        bool Create(ID3D11Device* device, const Vertex* vertices, uint32_t count);

        void Bind(ID3D11DeviceContext* ctx) const;
        uint32_t GetCount() const { return m_Count; }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
        uint32_t m_Stride = sizeof(Vertex);
        uint32_t m_Count  = 0;
    };

    // ── Index Buffer ────────────────────────────────────────────────────────
    class IndexBuffer {
    public:
        IndexBuffer() = default;

        bool Create(ID3D11Device* device, const uint32_t* indices, uint32_t count);

        void Bind(ID3D11DeviceContext* ctx) const;
        uint32_t GetCount() const { return m_Count; }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
        uint32_t m_Count = 0;
    };

} // namespace Nova
