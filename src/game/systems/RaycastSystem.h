//
// Created by bison on 01-01-23.
//

#ifndef PLATFORMER_RAYCASTSYSTEM_H
#define PLATFORMER_RAYCASTSYSTEM_H

#include <ECS.h>
#include "../components/CollisionComponent.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../components/ActorComponent.h"

using namespace ECS;
using namespace Renderer;

class RaycastSystem : public EntitySystem {
public:
    explicit RaycastSystem() = default;
    ~RaycastSystem() override = default;

protected:
    bool performRaycast(World *world, Vector2& rayOrigin, Vector2& rayEnd, RaycastHit& hit, bool checkLadders);
    bool performRaycastLadder(World *world, Vector2& rayOrigin, Vector2& rayEnd, RaycastHit& hit);
    bool performRaycastActor(World *world, Vector2& rayOrigin, Vector2& rayEnd, RaycastHit& hit);

    void updateRaycastOrigins(CollisionComponent &raycast, FloatRect b);
    void calculateRaySpacing(CollisionComponent &raycast, FloatRect b);
};


#endif //PLATFORMER_RAYCASTSYSTEM_H
