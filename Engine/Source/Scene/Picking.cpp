#include "Scene/Picking.h"
#include "Core/Logger.h"

namespace Nova {

    Ray Picking::ScreenToRay(
        int mouseX, int mouseY,
        int screenWidth, int screenHeight,
        const XMMATRIX& view,
        const XMMATRIX& projection)
    {
        // Convert mouse position to normalized device coordinates [-1, 1]
        float ndcX = (2.0f * mouseX / screenWidth) - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY / screenHeight);  // Flip Y

        // Create near and far points in clip space
        XMVECTOR nearPoint = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
        XMVECTOR farPoint  = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

        // Inverse view-projection
        XMMATRIX viewProj = view * projection;
        XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

        // Unproject to world space
        XMVECTOR worldNear = XMVector3TransformCoord(nearPoint, invViewProj);
        XMVECTOR worldFar  = XMVector3TransformCoord(farPoint,  invViewProj);

        // Ray direction
        XMVECTOR dir = XMVector3Normalize(XMVectorSubtract(worldFar, worldNear));

        return { worldNear, dir };
    }

    bool Picking::RayIntersectsAABB(
        XMVECTOR rayOrigin,
        XMVECTOR rayDir,
        XMVECTOR aabbMin,
        XMVECTOR aabbMax,
        float& tHit)
    {
        // Slab method for ray-AABB intersection
        // Process each axis
        XMVECTOR invDir = XMVectorReciprocal(rayDir);

        XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(aabbMin, rayOrigin), invDir);
        XMVECTOR t2 = XMVectorMultiply(XMVectorSubtract(aabbMax, rayOrigin), invDir);

        XMVECTOR tMin = XMVectorMin(t1, t2);
        XMVECTOR tMax = XMVectorMax(t1, t2);

        // Find largest tMin and smallest tMax across all axes
        float tEnter = std::max(
            std::max(XMVectorGetX(tMin), XMVectorGetY(tMin)),
            XMVectorGetZ(tMin));
        float tExit = std::min(
            std::min(XMVectorGetX(tMax), XMVectorGetY(tMax)),
            XMVectorGetZ(tMax));

        if (tEnter > tExit || tExit < 0.0f) {
            return false;
        }

        tHit = tEnter >= 0.0f ? tEnter : tExit;
        return true;
    }

    float Picking::ClosestPointRayLine(
        XMVECTOR rayOrigin, XMVECTOR rayDir,
        XMVECTOR lineOrigin, XMVECTOR lineDir,
        float& outRayT, float& outLineT)
    {
        XMVECTOR w0 = XMVectorSubtract(lineOrigin, rayOrigin);
        XMVECTOR u = lineDir;
        XMVECTOR v = rayDir;

        float a = XMVectorGetX(XMVector3Dot(u, u));
        float b = XMVectorGetX(XMVector3Dot(u, v));
        float c = XMVectorGetX(XMVector3Dot(v, v));
        float d = XMVectorGetX(XMVector3Dot(u, w0));
        float e = XMVectorGetX(XMVector3Dot(v, w0));

        float denom = a * c - b * b;
        if (denom < 1e-5f) {
            outLineT = 0.0f;
            outRayT  = (b > c ? d / b : e / c);
        } else {
            outLineT = (b * e - c * d) / denom;
            outRayT  = (a * e - b * d) / denom;
        }

        XMVECTOR pLine = XMVectorAdd(lineOrigin, XMVectorScale(u, outLineT));
        XMVECTOR pRay  = XMVectorAdd(rayOrigin, XMVectorScale(v, outRayT));

        return XMVectorGetX(XMVector3Length(XMVectorSubtract(pLine, pRay)));
    }

    Entity* Picking::PickEntity(
        Scene& scene,
        int mouseX, int mouseY,
        int screenWidth, int screenHeight,
        const XMMATRIX& view,
        const XMMATRIX& projection)
    {
        Ray ray = ScreenToRay(mouseX, mouseY, screenWidth, screenHeight, view, projection);

        Entity* closestEntity = nullptr;
        float closestT = std::numeric_limits<float>::max();

        for (auto& entityPtr : scene.GetEntities()) {
            Entity* entity = entityPtr.get();
            const Transform& transform = entity->GetTransform();
            Vec3 half = entity->AABBHalfExtents;

            // Compute AABB in world space (axis-aligned around entity position)
            Vec3 pos = transform.Position;
            Vec3 scale = transform.Scale;

            XMVECTOR aabbMin = XMVectorSet(
                pos.x - half.x * scale.x,
                pos.y - half.y * scale.y,
                pos.z - half.z * scale.z,
                0.0f);
            XMVECTOR aabbMax = XMVectorSet(
                pos.x + half.x * scale.x,
                pos.y + half.y * scale.y,
                pos.z + half.z * scale.z,
                0.0f);

            float tHit;
            if (RayIntersectsAABB(ray.Origin, ray.Direction, aabbMin, aabbMax, tHit)) {
                if (tHit < closestT) {
                    closestT = tHit;
                    closestEntity = entity;
                }
            }
        }

        return closestEntity;
    }

} // namespace Nova
