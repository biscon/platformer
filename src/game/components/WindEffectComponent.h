//
// Created by bison on 11-01-23.
//

#ifndef PLATFORMER_WINDEFFECTCOMPONENT_H
#define PLATFORMER_WINDEFFECTCOMPONENT_H

#include "../JsonUtil.h"

struct WindEffectComponent {
    WindEffectComponent() = default;
    explicit WindEffectComponent(const json& e) {
        speed = e["speed"];
        minStrength = e["minStrength"];
        maxStrength = e["maxStrength"];
        strengthScale = e["strengthScale"];
        interval = e["interval"];
        detail = e["detail"];
        distortion = e["distortion"];
        heightOffset = e["heightOffset"];
    }

    void save(json& e) {
        json j;
        j["speed"] = speed;
        j["minStrength"] = minStrength;
        j["maxStrength"] = maxStrength;
        j["strengthScale"] = strengthScale;
        j["interval"] = interval;
        j["detail"] = detail;
        j["distortion"] = distortion;
        j["heightOffset"] = heightOffset;
        e["windEffect"] = j;
    }

    float speed = 1.0f;
    float minStrength = 0.1f;
    float maxStrength = 0.5f;
    float strengthScale = 100.0;
    float interval = 3.5;
    float detail = 1.0;
    float distortion = 0; // hint_range(0.0, 1.0);
    float heightOffset = 0.0;
};

#endif //PLATFORMER_WINDEFFECTCOMPONENT_H
