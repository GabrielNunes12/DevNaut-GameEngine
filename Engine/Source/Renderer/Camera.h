#pragma once

#include "Math/NovaMath.h"

namespace Nova {

    class Camera {
    public:
        Camera() = default;

        void SetPerspective(float fovDegrees, float aspect, float nearZ, float farZ);
        void SetPosition(const Vec3& pos);
        void SetRotation(float pitchDeg, float yawDeg);
        void LookAt(const Vec3& target); // Optional helper

        XMMATRIX GetViewMatrix() const;
        XMMATRIX GetProjectionMatrix() const;
        XMMATRIX GetViewProjectionMatrix() const;

        const Vec3& GetPosition() const { return m_Position; }
        const Vec3& GetForward() const { return m_Forward; }
        const Vec3& GetRight() const { return m_Right; }
        const Vec3& GetUp() const { return m_Up; }

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

    private:
        void UpdateVectors();

        Vec3 m_Position = { 0.0f, 0.0f, -5.0f };
        Vec3 m_Target   = { 0.0f, 0.0f,  0.0f };
        Vec3 m_Up       = { 0.0f, 1.0f,  0.0f };
        Vec3 m_Right    = { 1.0f, 0.0f,  0.0f };
        Vec3 m_Forward  = { 0.0f, 0.0f,  1.0f };

        float m_Pitch = 0.0f; // Degrees
        float m_Yaw   = 0.0f; // Degrees

        float m_FovRadians = ToRadians(60.0f);
        float m_Aspect     = 16.0f / 9.0f;
        float m_NearZ      = 0.1f;
        float m_FarZ       = 100.0f;
    };

} // namespace Nova
