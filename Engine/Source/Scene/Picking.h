#pragma once

#include "Math/NovaMath.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include <limits>

namespace Nova {

    struct Ray {
        XMVECTOR Origin;
        XMVECTOR Direction;
    };

    class Picking {
    public:
        // Convert screen-space mouse coordinates to a world-space ray
        static Ray ScreenToRay(
            int mouseX, int mouseY,
            int screenWidth, int screenHeight,
            const XMMATRIX& view,
            const XMMATRIX& projection
        );

        // Test if a ray intersects an axis-aligned bounding box (AABB)
        // Returns true if hit, and writes the hit distance to tHit
        static bool RayIntersectsAABB(
            XMVECTOR rayOrigin,
            XMVECTOR rayDir,
            XMVECTOR aabbMin,
            XMVECTOR aabbMax,
            float& tHit
        );

        // Find the closest points between a Ray and a Line segment (used for Gizmo dragging)
        // rayOrigin/rayDir: The unnormalized or normalized ray
        // lineOrigin/lineDir: The line
        // outRayT: the distance along the ray
        // outLineT: the distance along the line
        // returns: the minimum distance between the ray and the line
        static float ClosestPointRayLine(
            XMVECTOR rayOrigin, XMVECTOR rayDir,
            XMVECTOR lineOrigin, XMVECTOR lineDir,
            float& outRayT, float& outLineT
        );

        // Pick the closest entity in the scene under the mouse cursor
        static Entity* PickEntity(
            Scene& scene,
            int mouseX, int mouseY,
            int screenWidth, int screenHeight,
            const XMMATRIX& view,
            const XMMATRIX& projection
        );
    };

} // namespace Nova
