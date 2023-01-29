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
    float width = 1920;
    float height = 1080;
    Renderer::Color skyColor = {33.0f/255.0f, 109.0f/255.0f, 154.0f/255.0f, 0.0f};
    Renderer::Color ambientColor = WHITE;
    std::string filename;
    float screenWidth = 1920;
    float screenHeight = 1080;
    std::vector<Spawn> spawns;
};

#endif //PLATFORMER_LEVELCONFIG_H
