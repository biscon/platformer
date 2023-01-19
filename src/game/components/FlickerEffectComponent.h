//
// Created by bison on 11-01-23.
//

#ifndef PLATFORMER_FLICKERCOMPONENT_H
#define PLATFORMER_FLICKERCOMPONENT_H

#include "../JsonUtil.h"

struct FlickerEffectComponent {
    FlickerEffectComponent() = default;
    explicit FlickerEffectComponent(const json& e) {
        minDuration = e["minDuration"];
        maxDuration = e["maxDuration"];
        if(e.contains("on")) {
            on = e["on"];
        }
    }

    void save(json& e) {
        json j;
        j["minDuration"] = minDuration;
        j["maxDuration"] = maxDuration;
        j["on"] = on;
        e["flickerEffect"] = j;
    }

    float minDuration = 0.05f;
    float maxDuration = 1.0f;
    float timer = 0;
    bool on = false;
};


#endif //PLATFORMER_FLICKERCOMPONENT_H
