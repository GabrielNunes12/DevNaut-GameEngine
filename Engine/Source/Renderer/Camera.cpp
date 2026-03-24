#include "Renderer/Camera.h"

namespace Nova {

    void Camera::SetPerspective(float fovDegrees, float aspect, float nearZ, float farZ) {
        m_FovRadians = ToRadians(fovDegrees);
        m_Aspect = aspect;
        m_NearZ  = nearZ;
        m_FarZ   = farZ;
    }

    void Camera::SetPosition(const Vec3& pos) {
        m_Position = pos;
        UpdateVectors();
    }

    void Camera::SetRotation(float pitchDeg, float yawDeg) {
        m_Pitch = pitchDeg;
        m_Yaw = yawDeg;

        // Clamp pitch to avoid look-at flips
        if (m_Pitch >  89.0f) m_Pitch =  89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;

        UpdateVectors();
    }

    void Camera::LookAt(const Vec3& target) {
        m_Target = target;
        // In a full implementation, we'd calculate Pitch/Yaw from Target.
        // For our Freecam, we primarily rely on SetRotation.
    }

    void Camera::UpdateVectors() {
        XMMATRIX rot = XMMatrixRotationRollPitchYaw(ToRadians(m_Pitch), ToRadians(m_Yaw), 0.0f);
        XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rot);
        XMVECTOR right   = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rot);
        XMVECTOR up      = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rot);

        XMStoreFloat3(&m_Target, XMLoadFloat3(&m_Position) + forward);
        XMStoreFloat3(&m_Forward, forward);
        XMStoreFloat3(&m_Right, right);
        XMStoreFloat3(&m_Up, up);
    }

    XMMATRIX Camera::GetViewMatrix() const {
        return MakeLookAt(m_Position, m_Target, m_Up);
    }

    XMMATRIX Camera::GetProjectionMatrix() const {
        return MakePerspective(m_FovRadians, m_Aspect, m_NearZ, m_FarZ);
    }

    XMMATRIX Camera::GetViewProjectionMatrix() const {
        return GetViewMatrix() * GetProjectionMatrix();
    }

} // namespace Nova
