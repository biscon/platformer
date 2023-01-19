//
// Created by bison on 22-11-22.
//

#ifndef GAME_ACTORSYSTEM_H
#define GAME_ACTORSYSTEM_H
#include <ECS.h>
#include "../components/SpriteComponent.h"
#include "../components/ActorComponent.h"
#include "../components/InputComponent.h"
#include "../components/CollisionInfoComponent.h"
#include "../components/PhysicComponent.h"

using namespace ECS;
using namespace Renderer;

class ActorSystem : public EntitySystem {
public:
    ~ActorSystem() override = default;
    void tick(World *world, float deltaTime) override;

private:
    void stateIdle(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime);
    void stateMove(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime);
    void stateJump(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime);
    void stateLadder(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime);

    void lerpVelocityX(PhysicComponent& physic, float target, float accTime,float deltaTime);
    void lerpVelocityY(PhysicComponent& physic, float target, float accTime,float deltaTime);
    const float minFallDistance = 50;
    const float maxJumpDistance = 30;
};


#endif //GAME_ACTORSYSTEM_H
