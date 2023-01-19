//
// Created by bison on 25-11-22.
//

#ifndef PLATFORMER_AABB_H
#define PLATFORMER_AABB_H

#include <Vector2.h>
#include "../renderer/Types.h"

using namespace Renderer;

struct AABB;

struct Hit {
    AABB *collider{};
    Vector2 pos;
    Vector2 delta;
    Vector2 normal;
    float time{};
};

struct Sweep {
    Hit hit;
    bool didHit = false;
    Vector2 pos;
    float time{};
};

struct AABB {
    AABB() = default;
    AABB(FloatRect& rect);

    Vector2 center;
    Vector2 half;

    bool intersectPos(const Vector2& point, Hit& hit);
    bool intersectSegment(const Vector2& pos, const Vector2& delta, float paddingX, float paddingY, Hit& hit);
    bool intersectAABB(const AABB& box, Hit& hit);
    void sweepAABB(const AABB& box, const Vector2& delta, Sweep& sweep);
    FloatRect toFloatRect() {
        return FloatRect(center.x - half.x, center.y - half.y, center.x + half.x, center.y + half.y);
    }
    void scale(float s) {

        half.x *= s;
        half.y *= s;
    }
};



#endif //PLATFORMER_AABB_H
