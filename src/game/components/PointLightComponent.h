//
// Created by bison on 06-01-23.
//

#ifndef PLATFORMER_LIGHTCOMPONENT_H
#define PLATFORMER_LIGHTCOMPONENT_H

#include <vector>
#include <ECS.h>
#include "../../renderer/Types.h"
#include "../JsonUtil.h"

using namespace Renderer;

struct PointLightComponent {
    PointLightComponent() = default;
    PointLightComponent(float innerRadius, float outerRadius, const Color &innerColor,
                   const Color &middleColor, const Color &outerColor, uint32_t segments);

    PointLightComponent(const json& e);
    void save(json& e);

    float innerRadius = 100;
    float outerRadius = 400;
    Color innerColor = {1.0f, 1.0f, 1.0f, 1.0f};
    Color middleColor = {0.75f, 0.75f, 0.75f, 0.75f};
    Color outerColor = {0.0f, 0.0f, 0.0f, 0.0f};
    u32 segments = 64;
    Vector2 posOffset;
    std::vector<float> innerMesh;
    std::vector<float> outerMesh;

    void rebuildMesh();
};


#endif //PLATFORMER_LIGHTCOMPONENT_H
