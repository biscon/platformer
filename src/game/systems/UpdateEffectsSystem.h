//
// Created by bison on 11-01-23.
//

#ifndef PLATFORMER_UPDATEEFFECTSSYSTEM_H
#define PLATFORMER_UPDATEEFFECTSSYSTEM_H

#include <ECS.h>
#include "../components/FlickerEffectComponent.h"
#include "../components/GlowEffectComponent.h"

using namespace ECS;
//using namespace Renderer;

class UpdateEffectsSystem : public EntitySystem  {
public:
    explicit UpdateEffectsSystem() = default;
    ~UpdateEffectsSystem() override = default;
    void tick(World* world, float deltaTime) override;

private:
    void updateFlicker(Entity* ent, float deltaTime);
    void updateGlow(Entity* ent, float deltaTime);
};


#endif //PLATFORMER_UPDATEEFFECTSSYSTEM_H
