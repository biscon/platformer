//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_DOORCOMPONENT_H
#define PLATFORMER_DOORCOMPONENT_H

#include <utility>

#include "../JsonUtil.h"

struct DoorComponent {
    explicit DoorComponent(const FloatRect &rect, std::string levelFileName, std::string spawnId) : rect(rect), levelFileName(std::move(levelFileName)), spawnId(std::move(spawnId)) {}
    explicit DoorComponent(const json& e) {
        rect = FloatRect(e["rect"][0], e["rect"][1], e["rect"][2], e["rect"][3]);
        levelFileName = e["levelFileName"];
        spawnId = e["spawnId"];
    }
    FloatRect rect;
    std::string levelFileName;
    std::string spawnId;

    void save(json& e) {
        json j;
        pushRect(j["rect"], rect);
        j["levelFileName"] = levelFileName;
        j["spawnId"] = spawnId;
        e["door"] = j;
    }
};


#endif //PLATFORMER_DOORCOMPONENT_H
