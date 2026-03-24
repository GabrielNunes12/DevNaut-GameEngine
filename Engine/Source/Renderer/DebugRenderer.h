#pragma once
#include "Renderer/DX11Context.h"
#include "Renderer/Shader.h"
#include "Math/NovaMath.h"
#include <wrl/client.h>
#include <vector>
#include <memory>

namespace Nova {

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct DebugVertex {
        Vec3 Position;
        Vec4 Color;
    };

    class DebugRenderer {
    public:
        DebugRenderer();
        ~DebugRenderer() = default;

        bool Init(ID3D11Device* device);
        
        void DrawLine(const Vec3& start, const Vec3& end, const Vec4& color);
        void DrawLine(const Vec3& start, const Vec3& end, uint32_t colorRGBA);

        void Flush(ID3D11DeviceContext* ctx, const DirectX::XMMATRIX& viewProjection);

    private:
        std::unique_ptr<Shader> m_Shader;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_ConstantBuffer;

        std::vector<DebugVertex> m_Vertices;
        uint32_t m_MaxVertices = 10000;
    };

} // namespace Nova
