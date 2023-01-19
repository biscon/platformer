//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_LADDERCOMPONENT_H
#define PLATFORMER_LADDERCOMPONENT_H

#include "../JsonUtil.h"

struct LadderComponent {
    explicit LadderComponent(const FloatRect &rect) : rect(rect) {}
    explicit LadderComponent(const json& e) {
        rect = FloatRect(e["rect"][0], e["rect"][1], e["rect"][2], e["rect"][3]);
    }
    FloatRect rect;

    void save(json& e) {
        json j;
        pushRect(j["rect"], rect);
        e["ladder"] = j;
    }
};


#endif //PLATFORMER_LADDERCOMPONENT_H
