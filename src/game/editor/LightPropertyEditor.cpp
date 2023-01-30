//
// Created by bison on 30-01-23.
//

#include "LightPropertyEditor.h"
#include "../components/PointLightComponent.h"
#include "../components/TransformComponent.h"
#include "../PolyUtil.h"
#include "ToolUtil.h"

void LightPropertyEditor::show() {
    render();
    if(movingHandle) {
        moveHandle();
    }
    if(moving) {
        move();
    }
}

void LightPropertyEditor::setSelected(Entity *ent) {
    if(selected != ent) {
        selectedHandle = -1;
        movingHandle = false;
        moving = false;
    }
    selected = ent;
    if(selected) {
        setHandles();
    }
}

bool LightPropertyEditor::onLeftDown(Vector2 pos) {
    if (selectedHandle == -1) {
        selectedHandle = selectHandle(pos);
        if (selectedHandle != -1) {
            SDL_Log("Selected light handle index = %d", selectedHandle);
        }
    }
    if (selectedHandle != -1) {
        auto vertex = handles[selectedHandle];
        auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                              vertex.x + 5, vertex.y + 5);
        rect.round();
        if (rect.containsPoint(pos.x, pos.y)) {
            startMovingHandle(pos, vertex);
            return true;
        } else {
            selectedHandle = selectHandle(pos);
            if (selectedHandle != -1) {
                startMovingHandle(pos, handles[selectedHandle]);
                return true;
            }
        }
    }
    auto transform = selected->get<TransformComponent>();
    auto light = selected->get<PointLightComponent>();
    if(transform.isValid() && light.isValid()) {
        if (pointInCircle(pos, transform->pos, light->outerRadius)) {
            if (!moving) {
                moving = true;
                moveStart = pos;
                snapToGrid(moveStart);
                snapToGrid(transform->pos);
                orgPos = transform->pos;
                SDL_Log("Start moving light");
                return true;
            }
        }
    }
    return false;
}

bool LightPropertyEditor::onLeftUp(Vector2 pos) {
    if(movingHandle) {
        movingHandle = false;
    }
    if(moving) {
        moving = false;
    }
    return false;
}

bool LightPropertyEditor::onAction(const Action &action) {
    return false;
}

void LightPropertyEditor::startMovingHandle(Vector2& pos, Vector2 vertex) {
    SDL_Log("start move handle");
    movingHandle = true;
    moveStart = pos;
    //snapToGrid(moveStart);
    //snapToGrid(vertex);
    orgVertex = vertex;
    handles[selectedHandle] = vertex;
}

void LightPropertyEditor::render() {
    auto light = selected->get<PointLightComponent>();
    auto transform = selected->get<TransformComponent>();
    if(!light.isValid() || !transform.isValid()) {
        return;
    }
    auto pos = transform->pos;
    pos.x -= camera.scrollX;
    pos.y -= camera.scrollY;
    buffers.unlit.pushCircle(pos, light->innerRadius, light->segments, WHITE);
    buffers.unlit.pushCircle(pos, light->outerRadius, light->segments, WHITE);

    // render handles
    for(u32 i = 0; i < handles.size(); ++i) {
        renderHandle(handles[i], selectedHandle == i ? GREEN : YELLOW);
    }
    //renderHandle(pos, YELLOW);

}

void LightPropertyEditor::renderHandle(Vector2 vertex, const Color &color) {
    vertex.x -= camera.scrollX;
    vertex.y -= camera.scrollY;
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffers.unlit.pushRect(rect, color);
}

void LightPropertyEditor::setHandles() {
    auto light = selected->get<PointLightComponent>();
    auto transform = selected->get<TransformComponent>();
    handles.clear();
    handles.emplace_back(Vector2(transform->pos.x + light->innerRadius, transform->pos.y));
    handles.emplace_back(Vector2(transform->pos.x + light->outerRadius, transform->pos.y));
}

void LightPropertyEditor::moveHandle() {
    auto &mouse = inputDevice.getMouseState();

    Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
    //snapToGrid(mouseDelta);
    mouseDelta -= moveStart;
    if(mouseDelta.x != 0) {
        handles[selectedHandle] = orgVertex;
        //SDL_Log("Moving dx,dy = %.2f,%.2f", mouseDelta.x, mouseDelta.y);
        handles[selectedHandle].x += mouseDelta.x;
    }
    auto v = handles[selectedHandle];
    buffers.unlit.pushText(string_format("%.2f,%.2f", v.x, v.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
    updateLight();
}

void LightPropertyEditor::updateLight() {
    auto transform = selected->get<TransformComponent>();
    auto light = selected->get<PointLightComponent>();
    light->innerRadius = handles[0].x - transform->pos.x;
    light->outerRadius = handles[1].x - transform->pos.x;
    light->rebuildMesh();
}


i32 LightPropertyEditor::selectHandle(const Vector2& pos) {
    for(i32 i = 0; i < handles.size(); ++i) {
        Vector2& vertex = handles[i];
        auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                              vertex.x + 5, vertex.y + 5);
        rect.round();
        if(rect.containsPoint(pos.x, pos.y)) {
            return i;
        }
    }
    return -1;
}

void LightPropertyEditor::move() {
    auto &mouse = inputDevice.getMouseState();
    Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
    snapToGrid(mouseDelta);
    mouseDelta -= moveStart;
    auto transform = selected->get<TransformComponent>();
    auto light = selected->get<PointLightComponent>();
    if(light.isValid() && transform.isValid()) {
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            transform->pos = orgPos;
            transform->pos += mouseDelta;
        }
    }
    buffers.unlit.pushText(string_format("+ %.2f,%.2f", mouseDelta.x, mouseDelta.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
}
