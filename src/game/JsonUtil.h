//
// Created by bison on 06-01-23.
//

#ifndef PLATFORMER_JSONUTIL_H
#define PLATFORMER_JSONUTIL_H

#include <json.hpp>
#include <Vector2.h>
#include "../renderer/Types.h"
#include "../renderer/Animation.h"

using json = nlohmann::json;
using namespace Renderer;

void pushColor(json& j, const Color& color);
void pushRect(json& j, const FloatRect& r);
void pushPos(json& j, const Vector2& p);
i32 repeatTypeToIndex(RepeatType rt);
RepeatType indexToRepeatType(i32 index);

#endif //PLATFORMER_JSONUTIL_H
