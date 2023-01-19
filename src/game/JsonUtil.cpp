//
// Created by bison on 06-01-23.
//

#include "JsonUtil.h"

void pushColor(json& j, const Color& color) {
    j.push_back(color.r);
    j.push_back(color.g);
    j.push_back(color.b);
    j.push_back(color.a);
}

void pushRect(json& j, const FloatRect& r) {
    j.push_back(r.left);
    j.push_back(r.top);
    j.push_back(r.right);
    j.push_back(r.bottom);
}

void pushPos(json& j, const Vector2& p) {
    j.push_back(p.x);
    j.push_back(p.y);
}


/*
void pushPath(json& j, const PathComponent& p) {
    j["reverse"] = p.reverse;
    json points;
    for(auto& pos : p.waypoints) {
        pushPos(points, pos);
    }
    j["waypoints"] = points;
}
 */
