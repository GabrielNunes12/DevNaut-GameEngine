#pragma once

// ── Nova Math ───────────────────────────────────────────────────────────────
// Thin wrapper around DirectXMath for convenience.

#include <DirectXMath.h>

namespace Nova {

    using namespace DirectX;

    // Aliases for clarity
    using Vec2 = XMFLOAT2;
    using Vec3 = XMFLOAT3;
    using Vec4 = XMFLOAT4;
    using Mat4 = XMFLOAT4X4;

    // ── Helper functions ────────────────────────────────────────────────────

    inline XMMATRIX MakePerspective(float fovY, float aspect, float nearZ, float farZ) {
        return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    }

    inline XMMATRIX MakeLookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        return XMMatrixLookAtLH(
            XMLoadFloat3(&eye),
            XMLoadFloat3(&target),
            XMLoadFloat3(&up)
        );
    }

    inline XMMATRIX MakeTranslation(float x, float y, float z) {
        return XMMatrixTranslation(x, y, z);
    }

    inline XMMATRIX MakeRotationX(float angle) { return XMMatrixRotationX(angle); }
    inline XMMATRIX MakeRotationY(float angle) { return XMMatrixRotationY(angle); }
    inline XMMATRIX MakeRotationZ(float angle) { return XMMatrixRotationZ(angle); }

    inline XMMATRIX MakeScale(float s) {
        return XMMatrixScaling(s, s, s);
    }

    constexpr float ToRadians(float degrees) {
        return degrees * 3.14159265358979f / 180.0f;
    }

} // namespace Nova
