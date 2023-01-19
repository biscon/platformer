//
// Created by bison on 21-12-22.
//

#include <SDL_log.h>
#include "TerrainTool.h"
#include "ToolUtil.h"
#include "../../util/string_util.h"
#include "../components/TransformComponent.h"

void TerrainTool::onUpdate() {
    if(newTerrain) {
        renderEditTerrain(newTerrain);
    } else if(selected) {
        renderEditTerrain(selected);
    }
    if(moving && selected) {
        moveVertex();
    }
    renderInfo();
}


bool TerrainTool::onKeyboard(MappedId key) {
    if(selected) {
        auto transform = selected->get<TransformComponent>();
        if (key == INPUT_ACTION_UP && transform.isValid()) {
            transform->layer += 1;
            return true;
        }
        if (key == INPUT_ACTION_DOWN && transform.isValid()) {
            transform->layer -= 1;
            return true;
        }
    }

    switch(key) {
        case INPUT_ACTION_DEL: return onDelete();
        case INPUT_ACTION_INS: return onInsert();
        case INPUT_ACTION_ENTER: return onEnter();
        case INPUT_ACTION_HIDE: return onHide();
        case INPUT_ACTION_UP: return onUp();
        default: return false;
    }
}

bool TerrainTool::onUp() {
    if(selected) {
        auto terrain = selected->get<TerrainComponent>();
        if(terrain.isValid()) {
            terrain->through = !terrain->through;
        }
        return true;
    }
    return false;
}

bool TerrainTool::onHide() {
    if(selected) {
        auto terrain = selected->get<TerrainComponent>();
        if(terrain.isValid()) {
            terrain->show = !terrain->show;
        }
        return true;
    }
    return false;
}

bool TerrainTool::onDelete() {
    if(newTerrain) {
        auto terrain = newTerrain->get<TerrainComponent>();
        if(terrain->polygon.size() > 0) {
            terrain->polygon.pop_back();
        }
        return true;
    } else if(selectedVertex != -1) {
        auto terrain = selected->get<TerrainComponent>();
        auto it = terrain->polygon.begin() + selectedVertex;
        terrain->polygon.erase(it);
        terrain->rebuildMesh();
        return true;
    }
    return false;
}

bool TerrainTool::onInsert() {
    if(selected && selectedVertex != -1) {
        auto terrain = selected->get<TerrainComponent>();
        Vector2 vertex = terrain->polygon[selectedVertex];
        if(selectedVertex > 0) {
            Vector2 edgeStart = terrain->polygon[selectedVertex-1];
            Vector2 edgeEnd = terrain->polygon[selectedVertex];
            vertex.x = (edgeStart.x + edgeEnd.x) / 2.0f;
            vertex.y = (edgeStart.y + edgeEnd.y) / 2.0f;
        }
        auto it = terrain->polygon.begin() + selectedVertex;
        terrain->polygon.insert(it, vertex);
        terrain->rebuildMesh();
        return true;
    }
    return false;
}

bool TerrainTool::onEnter() {
    if(newTerrain) {
        auto terrain = newTerrain->get<TerrainComponent>();
        terrain->rebuildMesh();
        newSelected = newTerrain;
        newTerrain = nullptr;
        return true;
    }
    return false;
}

bool TerrainTool::onLeftDown(Vector2 pos) {
    if(selected) {
        if(selectedVertex == -1) {
            selectedVertex = selectVertex(pos);
            if (selectedVertex != -1) {
                SDL_Log("Selected terrain vertex index = %d", selectedVertex);
            }
        }
        if(selectedVertex != -1) {
            auto terrain = selected->get<TerrainComponent>();
            auto vertex = terrain->polygon[selectedVertex];
            auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                                  vertex.x + 5, vertex.y + 5);
            rect.round();
            if(rect.containsPoint(pos.x, pos.y)) {
                startMovingVertex(pos, vertex, terrain.get());
                return true;
            } else {
                selectedVertex = selectVertex(pos);
                if(selectedVertex != -1) {
                    startMovingVertex(pos, terrain->polygon[selectedVertex], terrain.get());
                    return true;
                }
            }
        }
    } else {
        if(newTerrain) {
            SDL_Log("New vertex");
            snapToGrid(pos);
            auto terrain = newTerrain->get<TerrainComponent>();
            terrain->polygon.push_back(pos);
            return true;

        } else {
            SDL_Log("Start new terrain");
            snapToGrid(pos);
            newTerrain = world->create();
            newTerrain->assign<TransformComponent>(Vector2(0, 0), 1.0f, 0, 0);
            newTerrain->assign<TerrainComponent>(std::vector<Vector2>{{pos.x, pos.y}}, true);
            return true;
        }
    }
    return false;
}

void TerrainTool::startMovingVertex(Vector2& pos, Vector2 vertex, TerrainComponent& terrain) {
    SDL_Log("start move vertex");
    moving = true;
    moveStart = pos;
    snapToGrid(moveStart);
    snapToGrid(vertex);
    orgVertex = vertex;
    terrain.polygon[selectedVertex] = vertex;
    terrain.rebuildMesh();
}

bool TerrainTool::onLeftUp(Vector2 pos) {
    if(moving) {
        moving = false;
        auto terrain = selected->get<TerrainComponent>();
        if(terrain.isValid()) {
            SDL_Log("stop moving vertex");
            terrain->rebuildMesh();
        }
        return true;
    }
    return false;
}

void TerrainTool::reset() {
    selectedVertex = -1;
    if(newTerrain) {
        world->destroy(newTerrain, true);
        newTerrain = nullptr;
    }
    moving = false;
}

void TerrainTool::setSelected(Entity *ent) {
    selected = ent;
}

void TerrainTool::renderEditTerrain(Entity *ent) {
    Vector2 camOffset(-camera.scrollX, -camera.scrollY);
    auto terrain = ent->get<TerrainComponent>();
    if(terrain.isValid()) {
        Vector2 lineStart, lineEnd;
        for(i32 i = 0; i < terrain->polygon.size(); ++i) {
            lineStart = terrain->polygon[i];
            lineStart += camOffset;
            if (i == terrain->polygon.size() - 1) {
                lineEnd = terrain->polygon[0];
                lineEnd += camOffset;

                if(newTerrain) {
                    buffer.pushLine(lineStart, lineEnd, LAST_SEGMENT_COLOR);
                    renderVertex(lineStart, GREEN);
                } else {
                    buffer.pushLine(lineStart, lineEnd, WHITE);
                    renderVertex(lineStart, selectedVertex == i ? GREEN : YELLOW);
                }
                break;
            } else {
                lineEnd = terrain->polygon[i + 1];
                lineEnd += camOffset;
            }
            buffer.pushLine(lineStart, lineEnd, WHITE);
            renderVertex(lineStart, selectedVertex == i ? GREEN : YELLOW);
        }
    }
}

void TerrainTool::renderVertex(Vector2& vertex, const Color& color) {
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffer.pushRect(rect, color);
}

void TerrainTool::moveVertex() {
    auto &mouse = inputDevice.getMouseState();
    auto terrain = selected->get<TerrainComponent>();
    if(terrain.isValid()) {
        Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
        snapToGrid(mouseDelta);
        mouseDelta -= moveStart;
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            terrain->polygon[selectedVertex] = orgVertex;
            //SDL_Log("Moving dx,dy = %.2f,%.2f", mouseDelta.x, mouseDelta.y);
            terrain->polygon[selectedVertex] += mouseDelta;
            terrain->rebuildMesh();
        }
        auto v = terrain->polygon[selectedVertex];
        buffer.pushText(string_format("%.2f,%.2f", v.x, v.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
    }
}


i32 TerrainTool::selectVertex(Vector2& pos) {
    auto terrain = selected->get<TerrainComponent>();
    if(terrain.isValid()) {
        Vector2 vertex;
        for(i32 i = 0; i < terrain->polygon.size(); ++i) {
            vertex = terrain->polygon[i];
            auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                                  vertex.x + 5, vertex.y + 5);
            rect.round();
            if(rect.containsPoint(pos.x, pos.y)) {
                return i;
            }
        }
    }
    return -1;
}

Entity *TerrainTool::getNewSelected() {
    auto tmp = newSelected;
    newSelected = nullptr;
    return tmp;
}

void TerrainTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffer.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void TerrainTool::renderInfo() {
    if(selected) {
        auto terrain = selected->get<TerrainComponent>();
        renderTextRight("Select and drag vertices with the mouse", 1);
        renderTextRight("Insert vertex [INS]", 3);
        renderTextRight("Delete vertex [DEL]", 4);
        renderTextRight("Hide/Show terrain [H]", 5);
        if(terrain.isValid()) {
            renderTextRight(string_format("Jump through: %s [UP]", terrain->through ? "true" : "false"), 6);
        }
        auto transform = selected->get<TransformComponent>();
        if(transform.isValid()) {
            renderTextRight(string_format("Layer %d [UP/DOWN]", transform->layer), 7);
        }
    } else {
        renderTextRight("Click anywhere to start new terrain", 1);
        renderTextRight("Delete last vertex [DEL]", 3);
        renderTextRight("Finish terrain [ENTER]", 4);
    }


}


