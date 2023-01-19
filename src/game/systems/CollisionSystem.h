//
// Created by bison on 13-12-22.
//

#ifndef PLATFORMER_MOVESYSTEM_H
#define PLATFORMER_MOVESYSTEM_H

#include <ECS.h>
#include "../components/CollisionComponent.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../components/ActorComponent.h"
#include "RaycastSystem.h"
#include "../components/TerrainComponent.h"
#include "../components/PlatformComponent.h"
#include "../components/CollisionInfoComponent.h"
#include "../components/InputComponent.h"

using namespace ECS;
using namespace Renderer;

class CollisionSystem : public RaycastSystem {
public:
    explicit CollisionSystem(RenderCmdBuffer &buffer);

    ~CollisionSystem() override = default;

    void tick(World *world, float deltaTime) override;

private:
    void move(Entity *ent, Vector2 velocity);

    void
    verticalCollisions(World *world, CollisionComponent &raycast, CollisionInfoComponent &colInfo,
                       ActorComponent &actor, InputComponent &input, Vector2 &velocity);

    void
    horizontalCollisions(World *world, CollisionComponent &raycast, CollisionInfoComponent &colInfo,
                         Vector2 &velocity);

    void climbSlope(Vector2 &velocity, float slopeAngle, CollisionInfoComponent &colInfo);

    void descendSlope(World *world, CollisionComponent &raycast, CollisionInfoComponent &colInfo,
                      Vector2 &velocity);

    void slideDownMaxSlope(const RaycastHit& hit, Vector2& velocity, float maxSlopeAngle, CollisionInfoComponent& colInfo);

    void
    findDistanceGround(World *world, CollisionComponent &raycast, CollisionInfoComponent &colInfo,
                       Vector2 &velocity);

    void senseLadder(Entity *ent);

    void senseTopOfLadder(Entity *ent);

    void movePassengers(PlatformComponent &movingPlatform, bool moveBeforePlatform);

    RenderCmdBuffer &buffer;
};


#endif //PLATFORMER_MOVESYSTEM_H
