//
// Created by bison on 23-12-22.
//

#include <SDL_log.h>
#include "RectangleTool.h"
#include "ToolUtil.h"
#include "../../util/string_util.h"

void RectangleTool::onUpdate() {
    if(resizing) {
        resize();
    }
    if(drawing) {
        drawingRect();
    }
    if(rect) {
        renderRect();
    }
}

bool RectangleTool::onKeyboard(MappedId key) {
    return false;
}

bool RectangleTool::onLeftDown(Vector2 pos) {
    if(rect == nullptr && !selected && drawingEnabled) {
        SDL_Log("Starting rect");
        snapToGrid(pos);
        rect = std::make_unique<FloatRect>();
        rect->left = pos.x;
        rect->top = pos.y;
        rect->right = pos.x;
        rect->bottom = pos.y;
        drawing = true;
        return true;
    }
    if(selected && rect) {
        auto handle = FloatRect(rect->right - vertexHandleSize, rect->bottom - vertexHandleSize,
                              rect->right + vertexHandleSize, rect->bottom + vertexHandleSize);
        if(handle.containsPoint(pos.x, pos.y)) {
            SDL_Log("clicked handle, starting resize");
            startResizing(pos);
            return true;
        }
    }
    return false;
}

void RectangleTool::flipIfInverted() {
    // flip axis if rect is inverted
    if(rect->right < rect->left) {
        std::swap(rect->right, rect->left);
    }
    if(rect->bottom < rect->top) {
        std::swap(rect->bottom, rect->top);
    }
}

bool RectangleTool::onLeftUp(Vector2 pos) {
    if(drawing) {
        SDL_Log("Stopping rect");
        drawing = false;
        flipIfInverted();
        onRectComplete();
        //rect = nullptr;
        return true;
    }
    if(resizing) {
        SDL_Log("Stopping resize");
        resizing = false;
        flipIfInverted();
        onResizeComplete();
        return true;
    }
    return false;
}

void RectangleTool::reset() {
    drawing = false;
    resizing = false;
}

void RectangleTool::startResizing(Vector2& pos) {
    SDL_Log("start resizing");
    resizing = true;
    resizeStart = pos;
    snapToGrid(resizeStart);
    orgVertex = Vector2(rect->right, rect->bottom);
    snapToGrid(orgVertex);
}

void RectangleTool::resize() {
    auto &mouse = inputDevice.getMouseState();
    Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
    snapToGrid(mouseDelta);
    mouseDelta -= resizeStart;
    if(mouseDelta.x != 0 || mouseDelta.y != 0) {
        rect->right = orgVertex.x + mouseDelta.x;
        rect->bottom = orgVertex.y + mouseDelta.y;
        onResize();
    }
    buffer.pushText(string_format("+ %.2f,%.2f", mouseDelta.x, mouseDelta.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
}

void RectangleTool::drawingRect() {
    auto &mouse = inputDevice.getMouseState();
        Vector2 mousePos(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
        snapToGrid(mousePos);
        rect->right = mousePos.x;
        rect->bottom = mousePos.y;
        buffer.pushText(string_format("+ %.2f,%.2f", rect->width(), rect->height()), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
}

void RectangleTool::renderRect() {
    FloatRect r = *rect;
    r.translate(-camera.scrollX, -camera.scrollY);
    buffer.pushRect(r, WHITE);
    if(selected) {
        Vector2 pos(r.right, r.bottom);
        renderHandle(pos, YELLOW);
    }
}

void RectangleTool::renderHandle(Vector2& vertex, const Color& color) {
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffer.pushRect(rect, color);
}

