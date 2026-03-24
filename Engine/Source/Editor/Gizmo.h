#pragma once

#include "Math/NovaMath.h"
#include "Renderer/Buffer.h"

namespace Nova {

    class Entity;
    class Camera;
    class DX11Context;

    class Gizmo {
    public:
        Gizmo() = default;

        bool Init(ID3D11Device* device);

        // Bind the geometry buffers based on mode (0=Translate, 1=Rotate, 2=Scale)
        void Bind(ID3D11DeviceContext* ctx, int mode) const;

        uint32_t GetIndexCount(int mode) const;

    private:
        VertexBuffer m_TranslateVB;
        IndexBuffer  m_TranslateIB;

        VertexBuffer m_RotateVB;
        IndexBuffer  m_RotateIB;

        VertexBuffer m_ScaleVB;
        IndexBuffer  m_ScaleIB;
    };

} // namespace Nova
