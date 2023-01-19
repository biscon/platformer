//
// Created by bison on 25-11-22.
//

#ifndef PLATFORMER_UPDATEANIMATIONSYSTEM_H
#define PLATFORMER_UPDATEANIMATIONSYSTEM_H

#include <ECS.h>
#include "../components/SpriteComponent.h"

using namespace ECS;
using namespace Renderer;

class UpdateAnimationSystem : public EntitySystem {
public:
    UpdateAnimationSystem() = default;
    ~UpdateAnimationSystem() override = default;
    void tick(World *world, float deltaTime) override;

private:
    void update(SpriteComponent &sprite, float deltaTime);
};


#endif //PLATFORMER_UPDATEANIMATIONSYSTEM_H
