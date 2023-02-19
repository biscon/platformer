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

i32 repeatTypeToIndex(RepeatType rt) {
    switch(rt) {
        case RepeatType::Once:return 0;
        case RepeatType::Restart:return 1;
        case RepeatType::Reverse:return 2;
        case RepeatType::ReverseOnce:return 3;
    }
}

RepeatType indexToRepeatType(i32 index) {
    switch(index) {
        case 0:return RepeatType::Once;
        case 1:return RepeatType::Restart;
        case 2:return RepeatType::Reverse;
        case 3:return RepeatType::ReverseOnce;
        default: throw std::runtime_error("Invalid RepeatType index");
    }
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
