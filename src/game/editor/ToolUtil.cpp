//
// Created by bison on 21-12-22.
//

#include "ToolUtil.h"

INTERNAL i32 gridCellWidth  = 10;
INTERNAL i32 gridCellHeight = 10;

std::vector<i32> gridSizes = {1,5,10,20};

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

const std::vector<i32>& getGridSizes() {
    return gridSizes;
}

void setGridSize(i32 i) {
    gridCellWidth = gridSizes[i];
    gridCellHeight = gridSizes[i];
}