#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Core/Logger.h"

namespace Nova {

    template<typename T>
    class ConstantBuffer {
    public:
        ConstantBuffer() = default;

        bool Create(ID3D11Device* device) {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth      = (sizeof(T) + 15) & ~15; // 16-byte aligned
            desc.Usage           = D3D11_USAGE_DYNAMIC;
            desc.BindFlags       = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags  = D3D11_CPU_ACCESS_WRITE;

            HRESULT hr = device->CreateBuffer(&desc, nullptr, m_Buffer.GetAddressOf());
            if (FAILED(hr)) {
                NOVA_ENGINE_ERROR("ConstantBuffer::Create failed: 0x{:08X}", (unsigned)hr);
                return false;
            }
            return true;
        }

        void Update(ID3D11DeviceContext* ctx, const T& data) {
            D3D11_MAPPED_SUBRESOURCE mapped;
            HRESULT hr = ctx->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            if (SUCCEEDED(hr)) {
                memcpy(mapped.pData, &data, sizeof(T));
                ctx->Unmap(m_Buffer.Get(), 0);
            }
        }

        void BindVS(ID3D11DeviceContext* ctx, UINT slot = 0) const {
            ctx->VSSetConstantBuffers(slot, 1, m_Buffer.GetAddressOf());
        }

        void BindPS(ID3D11DeviceContext* ctx, UINT slot = 0) const {
            ctx->PSSetConstantBuffers(slot, 1, m_Buffer.GetAddressOf());
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
    };

} // namespace Nova
