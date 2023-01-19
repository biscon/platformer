//
// Created by bison on 01-01-23.
//

#ifndef PLATFORMER_MOVINGPLATFORMSYSTEM_H
#define PLATFORMER_MOVINGPLATFORMSYSTEM_H


#include <ECS.h>
#include "../components/CollisionComponent.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../components/ActorComponent.h"
#include "RaycastSystem.h"
#include "../components/TerrainComponent.h"
#include "../components/PlatformComponent.h"
#include "../components/TransformComponent.h"
#include "../components/PathComponent.h"

using namespace ECS;
using namespace Renderer;

class PlatformSystem : public RaycastSystem {
public:
    explicit PlatformSystem(RenderCmdBuffer &buffer) : buffer(buffer) {}
    ~PlatformSystem() override = default;
    void tick(World* world, float deltaTime) override;

private:
    Vector2 calcPlatformMovement(TransformComponent& transform, PlatformComponent& platform, PathComponent& path, float deltaTime);
    void calcPassengerMovement(Entity *ent, Vector2 velocity);
    RenderCmdBuffer& buffer;
};


#endif //PLATFORMER_MOVINGPLATFORMSYSTEM_H
