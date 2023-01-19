//
// Created by bison on 13-12-22.
//

#ifndef PLATFORMER_MOVECOMPONENT_H
#define PLATFORMER_MOVECOMPONENT_H

#include <ECS.h>
#include "../../renderer/Types.h"
#include "../JsonUtil.h"

using namespace Renderer;

struct RaycastHit {
    Vector2 pos;
    Vector2 normal;
    float distance = 0;
    ECS::Entity* target = nullptr;
};

struct RaycastOrigins {
    Vector2 topLeft, topRight;
    Vector2 bottomLeft, bottomRight;
};

struct CollisionComponent {
    RaycastOrigins raycastOrigins;
    float skinWidth = 2.0f;
    i32 horizontalRayCount = 32;
    i32 verticalRayCount = 8;
    float horizontalRaySpacing = 0;
    float verticalRaySpacing = 0;
    float maxSlopeAngle = 50.0f;

    void save(json& e);
};


#endif //PLATFORMER_MOVECOMPONENT_H
