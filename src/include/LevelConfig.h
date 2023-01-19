//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_LEVELCONFIG_H
#define PLATFORMER_LEVELCONFIG_H

#include "../renderer/Types.h"

using namespace Renderer;

struct Spawn {
    std::string id;
    Vector2 pos;
};

struct LevelConfig {
    float width = 2*1920;
    float height = 2*1080;
    Renderer::Color skyColor = BLUE;
    Renderer::Color ambientColor = WHITE;
    std::string filename;
    float screenWidth = 1920;
    float screenHeight = 1080;
    std::vector<Spawn> spawns;
};

#endif //PLATFORMER_LEVELCONFIG_H
