#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace Nova {
    enum class ColliderType { None, Box, Sphere };

    struct ColliderComponent {
        ColliderType Type = ColliderType::None;
        reactphysics3d::Collider* ColliderPtr = nullptr;
        reactphysics3d::CollisionShape* ShapePtr = nullptr;
        
        // Extents
        float BoxExtents[3] = { 0.5f, 0.5f, 0.5f };
        float SphereRadius = 0.5f;
    };
}
