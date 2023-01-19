//
// Created by bison on 08-01-23.
//

#ifndef PLATFORMER_VERLETMESHCOMPONENT_H
#define PLATFORMER_VERLETMESHCOMPONENT_H

#include <vector>
#include <ECS.h>
#include <Vector2.h>
#include "../../renderer/Types.h"
#include "../JsonUtil.h"

using namespace Renderer;

struct VerletPoint {
    VerletPoint(const Vector2 &position, bool locked) : position(position), locked(locked) {
        prevPosition = position;
    }

    Vector2 position;
    bool locked;
    Vector2 prevPosition;
};

struct VerletStick {
    VerletStick(u32 indexA, u32 indexB, bool lockLength, std::vector<VerletPoint>& points);

    u32 indexA;
    u32 indexB;
    float length;
    bool lockLength = false;
};

struct VerletMeshComponent {
    VerletMeshComponent() = default;

    explicit VerletMeshComponent(const json& e);

    std::vector<VerletPoint> points;
    std::vector<VerletStick> sticks;
    std::vector<u32> order;
    float gravity = 1000;
    u32 solveIterations = 5;
    Vector2 attachOffset = {0.0f, -50.0f};

    Vector2 topLeft, topRight, bottomRight, bottomLeft;

    void buildRope(Vector2 translate);

    void save(json& e);
};


#endif //PLATFORMER_VERLETMESHCOMPONENT_H
