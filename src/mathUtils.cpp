#include <cmath>
#include <cmath>
#include "mathUtils.hpp"


Vector2 math::NormalizeVec(const Vector2& v) {
    float len = sqrtf(v.x*v.x + v.y * v.y);
    if (len < 1e-6) return {0.0f, 0.0f};

    return {v.x / len, v.y / len};
}

float math::Dot(const Vector2& a, const Vector2& b){
    return a.x * b.x + a.y * b.y;
}

float math::CrossZ(const Vector2& a, const Vector2& b){
    return a.x * b.y - a.y * b.x;
}