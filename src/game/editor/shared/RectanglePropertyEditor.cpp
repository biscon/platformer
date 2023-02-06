//
// Created by bison on 30-01-23.
//

#include "RectanglePropertyEditor.h"
#include "../ToolUtil.h"

void RectanglePropertyEditor::show() {
    if(resizing) {
        resize();
    }
    if(moving) {
        move();
    }
    if(rect) {
        renderRect();
    }
}

bool RectanglePropertyEditor::onAction(const Input::Action &action) {
    return false;
}

bool RectanglePropertyEditor::onLeftDown(Vector2 pos) {
    if(selected && rect) {
        auto handle = FloatRect(rect->right - vertexHandleSize, rect->bottom - vertexHandleSize,
                                rect->right + vertexHandleSize, rect->bottom + vertexHandleSize);
        if(handle.containsPoint(pos.x, pos.y)) {
            SDL_Log("clicked handle, starting resize");
            startResizing(pos);
            return true;
        } else if(rect->containsPoint(pos.x, pos.y)) {
            SDL_Log("Start moving");
            startMoving(pos);
            return true;
        }
    }
    return false;
}

void RectanglePropertyEditor::flipIfInverted() {
    // flip axis if rect is inverted
    if(rect->right < rect->left) {
        std::swap(rect->right, rect->left);
    }
    if(rect->bottom < rect->top) {
        std::swap(rect->bottom, rect->top);
    }
}

bool RectanglePropertyEditor::onLeftUp(Vector2 pos) {
    if(resizing) {
        SDL_Log("Stopping resize");
        resizing = false;
        flipIfInverted();
        onResizeComplete();
        return true;
    }
    if(moving) {
        moving = false;
        return true;
    }
    return false;
}

void RectanglePropertyEditor::setSelected(Entity *ent) {
    if(ent != selected) {
        resizing = false;
    }
    selected = ent;
    buildRect();
}

void RectanglePropertyEditor::startMoving(const Vector2& pos) {
    if (!moving) {
        moving = true;
        moveStart = pos;
        snapToGrid(moveStart);
        orgPos = Vector2(rect->left, rect->top);
        snapToGrid(orgPos);
    }
}

void RectanglePropertyEditor::startResizing(Vector2& pos) {
    SDL_Log("start resizing");
    resizing = true;
    resizeStart = pos;
    snapToGrid(resizeStart);
    orgVertex = Vector2(rect->right, rect->bottom);
    snapToGrid(orgVertex);
}

void RectanglePropertyEditor::move() {
    auto &mouse = inputDevice.getMouseState();
    Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
    snapToGrid(mouseDelta);
    mouseDelta -= moveStart;

    if(mouseDelta.x != 0 || mouseDelta.y != 0) {
        Vector2 pos = orgPos;
        pos += mouseDelta;
        onMove(pos, mouseDelta);
    }
    buffer.pushText(string_format("+ %.2f,%.2f", mouseDelta.x, mouseDelta.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
}

void RectanglePropertyEditor::resize() {
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

void RectanglePropertyEditor::renderRect() {
    FloatRect r = *rect;
    r.translate(-camera.scrollX, -camera.scrollY);
    buffer.pushRect(r, WHITE);
    if(selected) {
        Vector2 pos(r.right, r.bottom);
        renderHandle(pos, YELLOW);
    }
}

void RectanglePropertyEditor::renderHandle(Vector2& vertex, const Color& color) {
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffer.pushRect(rect, color);
}
