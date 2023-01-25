//
// Created by bison on 21-12-22.
//

#include "ToolUtil.h"

INTERNAL float gridCellWidth  = 10;
INTERNAL float gridCellHeight = 10;

void snapToGrid(Vector2& pos) {
    snapToGrid(pos.x, pos.y);
}

void snapToGrid(float& x, float& y) {
    x = roundf(x / gridCellWidth)  * gridCellWidth;
    y = roundf(y / gridCellHeight) * gridCellHeight;
}

void snapToGrid(FloatRect& rect) {
    snapToGrid(rect.left, rect.top);
    snapToGrid(rect.right, rect.bottom);
}

void setGridSize(float w, float h) {
    gridCellWidth = w;
    gridCellHeight = h;
}