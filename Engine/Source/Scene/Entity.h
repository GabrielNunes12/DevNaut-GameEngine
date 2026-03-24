#pragma once

#include "Math/NovaMath.h"
#include <string>
#include <cstdint>
#include <optional>
#include "Physics/Rigidbody.h"
#include "Physics/Collider.h"

namespace Nova {

    // ── Transform ───────────────────────────────────────────────────────────
    struct Transform {
        Vec3 Position = { 0.0f, 0.0f, 0.0f };
        Vec3 Rotation = { 0.0f, 0.0f, 0.0f };  // Euler angles (radians)
        Vec3 Scale    = { 1.0f, 1.0f, 1.0f };

        // Build the world matrix: Scale * RotZ * RotY * RotX * Translation
        XMMATRIX GetWorldMatrix() const {
            return XMMatrixScaling(Scale.x, Scale.y, Scale.z)
                 * XMMatrixRotationX(Rotation.x)
                 * XMMatrixRotationY(Rotation.y)
                 * XMMatrixRotationZ(Rotation.z)
                 * XMMatrixTranslation(Position.x, Position.y, Position.z);
        }
    };

    // ── Entity Types ────────────────────────────────────────────────────────
    enum class EntityType {
        Mesh,
        Camera,
        Light,
        Empty
    };

    // ── Entity ──────────────────────────────────────────────────────────────
    class Entity {
    public:
        Entity(uint32_t id, const std::string& name, EntityType type = EntityType::Mesh)
            : m_Id(id), m_Name(name), Type(type) {}

        uint32_t           GetId()   const { return m_Id; }
        const std::string& GetName() const { return m_Name; }

        Transform&       GetTransform()       { return m_Transform; }
        const Transform& GetTransform() const { return m_Transform; }

        // AABB half-extents (defaults to unit cube; override for different shapes)
        Vec3 AABBHalfExtents = { 0.5f, 0.5f, 0.5f };

        // Type
        EntityType Type = EntityType::Mesh;

        // Physics
        RigidbodyComponent* GetRigidbody() { return m_Rigidbody.has_value() ? &m_Rigidbody.value() : nullptr; }
        ColliderComponent*  GetCollider()  { return m_Collider.has_value() ? &m_Collider.value() : nullptr; }

        RigidbodyComponent* AddRigidbody() { 
            if (!m_Rigidbody) m_Rigidbody = RigidbodyComponent(); 
            return &m_Rigidbody.value(); 
        }
        ColliderComponent* AddCollider(ColliderType type) { 
            if (!m_Collider) {
                m_Collider = ColliderComponent();
                m_Collider->Type = type;
            }
            return &m_Collider.value(); 
        }

        void RemoveRigidbody() { m_Rigidbody.reset(); }
        void RemoveCollider()  { m_Collider.reset(); }

        virtual void OnCollision(Entity* other);

    private:
        uint32_t    m_Id;
        std::string m_Name;
        Transform   m_Transform;

        std::optional<RigidbodyComponent> m_Rigidbody;
        std::optional<ColliderComponent>  m_Collider;
    };

} // namespace Nova
