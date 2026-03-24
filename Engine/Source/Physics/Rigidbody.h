#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace Nova {
    enum class RigidBodyType { Static, Kinematic, Dynamic };

    struct RigidbodyComponent {
        reactphysics3d::RigidBody* Body = nullptr;
        RigidBodyType Type = RigidBodyType::Dynamic;
        float Mass = 1.0f;
    };
}
