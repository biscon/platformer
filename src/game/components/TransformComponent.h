//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_TRANSFORMCOMPONENT_H
#define PLATFORMER_TRANSFORMCOMPONENT_H


#include <json.hpp>
#include <Vector2.h>
#include <defs.h>
#include "../JsonUtil.h"

using json = nlohmann::json;

struct TransformComponent {
    TransformComponent(const Vector2 &pos, float scale, float rotation, i32 layer) : pos(pos), scale(scale),
                                                                                     rotation(rotation), layer(layer) {}

    TransformComponent(const TransformComponent& old) {
        pos = old.pos;
        scale = old.scale;
        rotation = old.rotation;
        layer = old.layer;
    }

    explicit TransformComponent(const json& e) {
        pos = Vector2(e["pos"][0], e["pos"][1]);
        scale = e["scale"];
        rotation = e["rotation"];
        layer = e["layer"];
    }


    Vector2 pos;
    float scale;
    float rotation;
    i32 layer = 0;

    void save(json& e) {
        json j;
        pushPos(j["pos"], pos);
        j["scale"] = scale;
        j["rotation"] = rotation;
        j["layer"] = layer;
        e["transform"] = j;
    }
};

#endif //PLATFORMER_TRANSFORMCOMPONENT_H
