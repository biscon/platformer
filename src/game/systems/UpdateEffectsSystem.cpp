//
// Created by bison on 11-01-23.
//

#include <cglm/util.h>
#include <SDL_log.h>
#include "UpdateEffectsSystem.h"
#include "../PolyUtil.h"
#include "../Utils.h"


void UpdateEffectsSystem::tick(World *world, float deltaTime) {
    for (Entity* ent : world->all(false)) {
        if(ent->has<FlickerEffectComponent>()) {
            updateFlicker(ent, deltaTime);
        }
        if(ent->has<GlowEffectComponent>()) {
            updateGlow(ent, deltaTime);
        }
    }
}

void UpdateEffectsSystem::updateFlicker(Entity *ent, float deltaTime) {
    auto flicker = ent->get<FlickerEffectComponent>();
    if(flicker->timer > 0) {
        flicker->timer -= deltaTime;
    } else {
        flicker->timer = randomFloatRange(flicker->minDuration, flicker->maxDuration);
        flicker->on = !flicker->on;
    }


}

void UpdateEffectsSystem::updateGlow(Entity *ent, float deltaTime) {
    auto glow = ent->get<GlowEffectComponent>();
    if(glow->timer > 0) {
        glow->timer -= deltaTime;
    } else {
        glow->timer = glow->duration;
        glow->fraction = 0;
        std::swap(glow->start, glow->end);
    }
    auto fraction = glm_lerp(glow->start, glow->end, glow->timer / glow->duration);
    fraction = std::clamp(fraction, 0.0f, 1.0f);
    glow->fraction = ease(fraction, 1.0f);
    //SDL_Log("Glow frac: %f", glow->fraction);
}
