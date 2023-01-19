//
// Created by bison on 11-01-23.
//

#ifndef PLATFORMER_GLOWCOMPONENT_H
#define PLATFORMER_GLOWCOMPONENT_H

#include "../JsonUtil.h"

struct GlowEffectComponent {
    GlowEffectComponent() = default;
    explicit GlowEffectComponent(const json& e) {
        duration = e["duration"];
    }

    void save(json& e) {
        json j;
        j["duration"] = duration;
        e["glowEffect"] = j;
    }

    float duration = 5.0f;
    float timer = 0;
    float fraction = 0;
    float start = 0;
    float end = 1;
};


#endif
