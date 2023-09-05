//
// Created by bison on 12-12-22.
//

#include "TerrainComponent.h"
#include <earcut.h>
#include <SDL_log.h>

// The number type to use for tessellation
using Coord = float;
// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;

using Point = std::array<Coord, 2>;

TerrainComponent::TerrainComponent(std::vector<Vector2> polygon, bool show) : polygon(std::move(polygon)), show(show) {
    rebuildMesh();
}

TerrainComponent::TerrainComponent(const json &e) {
    show = e["show"];
    through = e["through"];
    if(e.contains("light"))
        light = e["light"];
    if(e.contains("color")) {
        color = { e["color"][0], e["color"][1], e["color"][2], e["color"][3] };
    }
    auto vertices = e["vertices"];
    for(i32 i = 0; i < vertices.size(); i += 2) {
        Vector2 vertex(vertices[i], vertices[i+1]);
        polygon.emplace_back(vertex);
        //std::cout << std::setw(4) << vertices[i] << std::endl;
    }
    rebuildMesh();
}

void TerrainComponent::rebuildMesh() {
    triangleMesh.clear();
    std::vector<std::vector<Point>> poly;
    std::vector<Point> points;
    points.reserve(polygon.size());
    for(auto& v : this->polygon) {
        points.push_back({roundf(v.x), roundf(v.y)});
    }
    poly.push_back(points);

    std::vector<N> indices = mapbox::earcut<N>(poly);
    //SDL_Log("ids: %d", (int) ids.size());
    for(auto& index : indices) {
        Point& p = poly.at(0).at(index);
        //SDL_Log("index: %d point: %.02f, %.02f", index, p[0], p[1]);
        triangleMesh.emplace_back(p[0],p[1]);
    }
}

void TerrainComponent::save(json &e) {
    json t;
    for (auto &vertex : polygon) {
        pushPos(t["vertices"], vertex);
    }
    t["show"] = show;
    t["through"] = through;
    t["light"] = light;
    pushColor(t["color"], color);
    e["terrain"] = t;
}
