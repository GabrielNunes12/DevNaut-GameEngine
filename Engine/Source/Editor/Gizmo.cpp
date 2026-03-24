#include "Editor/Gizmo.h"

namespace Nova {

    // Simple line-list geometry for 3 axes
    static Vertex g_GizmoVertices[] = {
        // X-axis (Red)
        { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        // Arrowhead lines (X)
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.8f, 0.1f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.8f,-0.1f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },

        // Y-axis (Green)
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        // Arrowhead lines (Y)
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.1f, 0.8f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { {-0.1f, 0.8f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },

        // Z-axis (Blue)
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        // Arrowhead lines (Z)
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 0.1f, 0.8f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
        { { 0.0f,-0.1f, 0.8f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }
    };

    static uint32_t g_GizmoIndices[] = {
        0, 1, 2, 3, 4, 5,       // X
        6, 7, 8, 9, 10, 11,     // Y
        12, 13, 14, 15, 16, 17  // Z
    };

    bool Gizmo::Init(ID3D11Device* device) {
        if (!m_TranslateVB.Create(device, g_GizmoVertices, _countof(g_GizmoVertices))) return false;
        if (!m_TranslateIB.Create(device, g_GizmoIndices, _countof(g_GizmoIndices))) return false;

        // -- Scale Gizmo --
        std::vector<Vertex> sV;
        std::vector<uint32_t> sI;
        auto addScaleAxis = [&](float r, float g, float b, int axis) {
            uint32_t startIdx = (uint32_t)sV.size();
            sV.push_back({ {0,0,0}, {r,g,b,1.0f}, {0,0,0} });
            Vec3 end = { axis==0?1.0f:0.0f, axis==1?1.0f:0.0f, axis==2?1.0f:0.0f };
            sV.push_back({ {end.x, end.y, end.z}, {r,g,b,1.0f}, {0,0,0} });
            sI.push_back(startIdx); sI.push_back(startIdx+1);

            // Draw a small box at the end
            float s = 0.05f;
            Vec3 p1 = { axis==0?1.0f:s, axis==1?1.0f:s, axis==2?1.0f:s };
            Vec3 p2 = { axis==0?1.0f:-s, axis==1?1.0f:s, axis==2?1.0f:-s };
            Vec3 p3 = { axis==0?1.0f:-s, axis==1?1.0f:-s, axis==2?1.0f:s };
            Vec3 p4 = { axis==0?1.0f:s, axis==1?1.0f:-s, axis==2?1.0f:-s };
            
            uint32_t bIdx = (uint32_t)sV.size();
            sV.push_back({ {p1.x, p1.y, p1.z}, {r,g,b,1.0f}, {0,0,0} });
            sV.push_back({ {p2.x, p2.y, p2.z}, {r,g,b,1.0f}, {0,0,0} });
            sV.push_back({ {p3.x, p3.y, p3.z}, {r,g,b,1.0f}, {0,0,0} });
            sV.push_back({ {p4.x, p4.y, p4.z}, {r,g,b,1.0f}, {0,0,0} });

            sI.push_back(bIdx); sI.push_back(bIdx+1);
            sI.push_back(bIdx+1); sI.push_back(bIdx+2);
            sI.push_back(bIdx+2); sI.push_back(bIdx+3);
            sI.push_back(bIdx+3); sI.push_back(bIdx);
        };
        addScaleAxis(1.0f, 0.0f, 0.0f, 0); // X
        addScaleAxis(0.0f, 1.0f, 0.0f, 1); // Y
        addScaleAxis(0.0f, 0.0f, 1.0f, 2); // Z
        if (!m_ScaleVB.Create(device, sV.data(), (uint32_t)sV.size())) return false;
        if (!m_ScaleIB.Create(device, sI.data(), (uint32_t)sI.size())) return false;

        // -- Rotate Gizmo --
        std::vector<Vertex> rV;
        std::vector<uint32_t> rI;
        int segments = 32;
        auto addRotateCircle = [&](float r, float g, float b, int axis) {
            uint32_t startIdx = (uint32_t)rV.size();
            for (int i = 0; i < segments; ++i) {
                float theta = (float)i / segments * 6.2831853f;
                float c = cosf(theta);
                float s = sinf(theta);
                Vec3 p = { axis==0?0.0f:c, axis==1?0.0f:(axis==0?c:s), axis==2?0.0f:s };
                rV.push_back({ {p.x, p.y, p.z}, {r,g,b,1.0f}, {0,0,0} });
                
                rI.push_back(startIdx + i);
                rI.push_back(startIdx + ((i + 1) % segments));
            }
        };
        addRotateCircle(1.0f, 0.0f, 0.0f, 0); // X
        addRotateCircle(0.0f, 1.0f, 0.0f, 1); // Y
        addRotateCircle(0.0f, 0.0f, 1.0f, 2); // Z
        if (!m_RotateVB.Create(device, rV.data(), (uint32_t)rV.size())) return false;
        if (!m_RotateIB.Create(device, rI.data(), (uint32_t)rI.size())) return false;

        return true;
    }

    void Gizmo::Bind(ID3D11DeviceContext* ctx, int mode) const {
        if (mode == 0) {
            m_TranslateVB.Bind(ctx);
            m_TranslateIB.Bind(ctx);
        } else if (mode == 1) {
            m_RotateVB.Bind(ctx);
            m_RotateIB.Bind(ctx);
        } else if (mode == 2) {
            m_ScaleVB.Bind(ctx);
            m_ScaleIB.Bind(ctx);
        }
    }

    uint32_t Gizmo::GetIndexCount(int mode) const {
        if (mode == 0) return m_TranslateIB.GetCount();
        if (mode == 1) return m_RotateIB.GetCount();
        if (mode == 2) return m_ScaleIB.GetCount();
        return 0;
    }

} // namespace Nova
