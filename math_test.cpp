#include <iostream>
#include <DirectXMath.h>

using namespace DirectX;

float ClosestPointRayLine(XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR lineOrigin, XMVECTOR lineDir, float& outRayT, float& outLineT) {
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

int main() {
    // Camera at 0, 0, -4. Looking down +Z.
    XMVECTOR rayPos = XMVectorSet(0, 0, -4, 1);
    XMVECTOR rayDir = XMVectorSet(0, 0, 1, 0); // straight ahead
    
    // Gizmo X axis at 0, 0, 0. Direction (1,0,0).
    XMVECTOR linePos = XMVectorSet(0, 0, 0, 1);
    XMVECTOR lineDir = XMVectorSet(1, 0, 0, 0);
    
    float rT, lT;
    float dist = ClosestPointRayLine(rayPos, rayDir, linePos, lineDir, rT, lT);
    
    std::cout << "Center click - Dist: " << dist << " rT: " << rT << " lT: " << lT << std::endl;

    // Shift mouse right. Ray rotated slightly right.
    // Let's say rayDir = (1, 0, 4) normalized.
    rayDir = XMVector3Normalize(XMVectorSet(1, 0, 4, 0));
    dist = ClosestPointRayLine(rayPos, rayDir, linePos, lineDir, rT, lT);
    
    std::cout << "Right click - Dist: " << dist << " rT: " << rT << " lT: " << lT << std::endl;
    
    return 0;
}
