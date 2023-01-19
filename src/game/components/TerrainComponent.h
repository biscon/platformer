//
// Created by bison on 12-12-22.
//

#ifndef PLATFORMER_TERRAINCOMPONENT_H
#define PLATFORMER_TERRAINCOMPONENT_H

#include <vector>
#include <ECS.h>
#include "../../renderer/Types.h"
#include "../JsonUtil.h"

using namespace Renderer;

struct TerrainComponent {
    TerrainComponent() = default;
    TerrainComponent(std::vector<Vector2> polygon, bool show);

    explicit TerrainComponent(const json& e);
    std::vector<Vector2> polygon;
    std::vector<Vector2> triangleMesh;
    bool show = true;
    bool through = false;
    bool light = true;
    Renderer::Color color = {0.13f, 0.13f, 0.13f, 1.0f};

    void rebuildMesh();
    void save(json& e);
};


#endif //PLATFORMER_TERRAINCOMPONENT_H
