//
// Created by bison on 06-01-23.
//

#include <SDL_log.h>
#include "LightTool.h"
#include "../../util/string_util.h"
#include "../components/PointLightComponent.h"
#include "ToolUtil.h"
#include "../components/TransformComponent.h"

void LightTool::onUpdate() {
    if(selected && canEdit) {
        render(selected);
        if(moving) {
            moveHandle();
        }
    }
    renderInfo();
}

bool LightTool::onKeyboard(MappedId key) {
    return false;
}

bool LightTool::onLeftDown(Vector2 pos) {
    if(!selected && !canEdit) {
        selected = world->create();
        selected->assign<TransformComponent>(pos, 1.0f, 0.0f, 0);
        selected->assign<PointLightComponent>();
        selected->get<PointLightComponent>()->rebuildMesh();
        newSelected = selected;
        setSelected(selected);
        return true;
    }
    if(selected && canEdit) {
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
    }
    return false;
}

void LightTool::startMovingHandle(Vector2& pos, Vector2 vertex) {
    SDL_Log("start move handle");
    moving = true;
    moveStart = pos;
    //snapToGrid(moveStart);
    //snapToGrid(vertex);
    orgVertex = vertex;
    handles[selectedHandle] = vertex;
}

bool LightTool::onLeftUp(Vector2 pos) {
    if(moving) {
        moving = false;
        SDL_Log("stop moving handle");
        return true;
    }
    return false;
}

void LightTool::reset() {
    selectedHandle = -1;
    moving = false;
    canEdit = false;
    if(selected) {
        canEdit = selected->has<TransformComponent, PointLightComponent>();
        if (canEdit) {
            setHandles();
        }
    }
}

void LightTool::setSelected(Entity *ent) {
    selected = ent;
    if(selected) {
        canEdit = selected->has<TransformComponent, PointLightComponent>();
        if (canEdit) {
            setHandles();
        }
    }
}

Entity *LightTool::getNewSelected() {
    auto tmp = newSelected;
    newSelected = nullptr;
    return tmp;
}

void LightTool::render(Entity* ent) {
    auto light = ent->get<PointLightComponent>();
    auto transform = ent->get<TransformComponent>();
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

void LightTool::renderHandle(Vector2 vertex, const Color &color) {
    vertex.x -= camera.scrollX;
    vertex.y -= camera.scrollY;
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffers.unlit.pushRect(rect, color);
}

void LightTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffers.unlit.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void LightTool::renderInfo() {
    if(selected && canEdit) {
        renderTextRight("Move handles with mouse", 1);
        renderTextRight("Delete light [DEL]", 3);
    } else {
        renderTextRight("Click anywhere to make new light", 1);
    }
}

void LightTool::setHandles() {
    auto light = selected->get<PointLightComponent>();
    auto transform = selected->get<TransformComponent>();
    handles.clear();
    handles.emplace_back(Vector2(transform->pos.x + light->innerRadius, transform->pos.y));
    handles.emplace_back(Vector2(transform->pos.x + light->outerRadius, transform->pos.y));
}

void LightTool::moveHandle() {
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

void LightTool::updateLight() {
    auto transform = selected->get<TransformComponent>();
    auto light = selected->get<PointLightComponent>();
    light->innerRadius = handles[0].x - transform->pos.x;
    light->outerRadius = handles[1].x - transform->pos.x;
    light->rebuildMesh();
}


i32 LightTool::selectHandle(const Vector2& pos) {
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