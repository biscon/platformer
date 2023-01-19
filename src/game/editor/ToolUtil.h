//
// Created by bison on 21-12-22.
//

#ifndef PLATFORMER_TOOLUTIL_H
#define PLATFORMER_TOOLUTIL_H

#include <Vector2.h>
#include "../../renderer/Types.h"
#include "vector"

using namespace Renderer;

void snapToGrid(Vector2& pos);
void snapToGrid(float& x, float& y);
void snapToGrid(FloatRect& rect);
const std::vector<i32>& getGridSizes();
void setGridSize(i32 i);

#endif //PLATFORMER_TOOLUTIL_H
