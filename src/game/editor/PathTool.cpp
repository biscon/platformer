//
// Created by bison on 21-12-22.
//

#include <SDL_log.h>
#include "PathTool.h"
#include "ToolUtil.h"
#include "../../util/string_util.h"

void PathTool::onUpdate() {
    if(selected) {
        if (selectedPath) {
            render(selectedPath);
        }
        if (moving) {
            moveVertex();
        }
    }
    renderInfo();
}

bool PathTool::onKeyboard(MappedId key) {
    switch(key) {
        case INPUT_ACTION_DEL: return onDelete();
        case INPUT_ACTION_INS: return onInsert();
        case INPUT_ACTION_ENTER: return onEnter();
        default: return false;
    }
}

bool PathTool::onDelete() {
    if(selectedVertex != -1) {
        auto it = selectedPath->waypoints.begin() + selectedVertex;
        selectedPath->waypoints.erase(it);
        return true;
    }
    if(selectedPath && makingNew) {
        selectedPath->waypoints.pop_back();
        return true;
    }
    return false;
}

bool PathTool::onInsert() {
    if(selectedPath && selectedVertex != -1) {
        Vector2 vertex = selectedPath->waypoints[selectedVertex];
        if(selectedVertex > 0) {
            Vector2 edgeStart = selectedPath->waypoints[selectedVertex-1];
            Vector2 edgeEnd = selectedPath->waypoints[selectedVertex];
            vertex.x = (edgeStart.x + edgeEnd.x) / 2.0f;
            vertex.y = (edgeStart.y + edgeEnd.y) / 2.0f;
        }
        auto it = selectedPath->waypoints.begin() + selectedVertex;
        selectedPath->waypoints.insert(it, vertex);
        return true;
    }
    return false;
}

bool PathTool::onEnter() {
    if(selected && selectedPath && makingNew) {
        makingNew = false;
        return true;
    }
    return false;
}

bool PathTool::onLeftDown(Vector2 pos) {
    if(selected) {
        if (selectedPath && !makingNew) {
            if (selectedVertex == -1) {
                selectedVertex = selectVertex(pos);
                if (selectedVertex != -1) {
                    SDL_Log("Selected path vertex index = %d", selectedVertex);
                }
            }
            if (selectedVertex != -1) {
                auto vertex = selectedPath->waypoints[selectedVertex];
                auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                                      vertex.x + 5, vertex.y + 5);
                rect.round();
                if (rect.containsPoint(pos.x, pos.y)) {
                    startMovingVertex(pos, vertex);
                    return true;
                } else {
                    selectedVertex = selectVertex(pos);
                    if (selectedVertex != -1) {
                        startMovingVertex(pos, selectedPath->waypoints[selectedVertex]);
                        return true;
                    }
                }
            }
        }
        if(makingNew) {
            SDL_Log("New vertex");
            snapToGrid(pos);
            selectedPath->waypoints.push_back(pos);
            return true;
        }
        if(!selectedPath){
            SDL_Log("Start new path");
            snapToGrid(pos);
            selected->assign<PathComponent>(std::vector<Vector2> {{pos.x, pos.y}}, true);
            selectedPath = &selected->get<PathComponent>().get();
            makingNew = true;
            return true;
        }
    }
    return false;
}

void PathTool::startMovingVertex(Vector2& pos, Vector2 vertex) {
    SDL_Log("start move vertex");
    moving = true;
    moveStart = pos;
    snapToGrid(moveStart);
    snapToGrid(vertex);
    orgVertex = vertex;
    selectedPath->waypoints[selectedVertex] = vertex;
}

bool PathTool::onLeftUp(Vector2 pos) {
    if(moving) {
        moving = false;
        SDL_Log("stop moving vertex");
        return true;
    }
    return false;
}

void PathTool::reset() {
    selectedVertex = -1;
    moving = false;
}

void PathTool::setSelected(Entity *ent) {
    reset();
    selected = ent;
    if(selected) {
        auto path = selected->get<PathComponent>();
        if (path.isValid()) {
            selectedPath = &path.get();
        } else {
            selectedPath = nullptr;
        }
    } else {
        selectedPath = nullptr;
    }
}

void PathTool::render(PathComponent* path) {
    if(path->waypoints.empty()) {
        return;
    }
    Vector2 camOffset(-camera.scrollX, -camera.scrollY);
    Vector2 lineStart, lineEnd;
    for(i32 i = 0; i < path->waypoints.size(); ++i) {
        lineStart = path->waypoints[i];
        lineStart += camOffset;
        if (i == path->waypoints.size() - 1) {
            lineEnd = path->waypoints[0];
            lineEnd += camOffset;

            buffer.pushLine(lineStart, lineEnd, LAST_SEGMENT_COLOR);
            renderVertex(lineStart, selectedVertex == i ? GREEN : YELLOW);
            break;
        } else {
            lineEnd = path->waypoints[i + 1];
            lineEnd += camOffset;
        }
        buffer.pushLine(lineStart, lineEnd, makingNew ? RED : WHITE);
        renderVertex(lineStart, selectedVertex == i ? GREEN : YELLOW);
    }
}

void PathTool::renderVertex(Vector2& vertex, const Color& color) {
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffer.pushRect(rect, color);
}

void PathTool::moveVertex() {
    auto &mouse = inputDevice.getMouseState();
    
    Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
    snapToGrid(mouseDelta);
    mouseDelta -= moveStart;
    if(mouseDelta.x != 0 || mouseDelta.y != 0) {
        selectedPath->waypoints[selectedVertex] = orgVertex;
        //SDL_Log("Moving dx,dy = %.2f,%.2f", mouseDelta.x, mouseDelta.y);
        selectedPath->waypoints[selectedVertex] += mouseDelta;
    }
    auto v = selectedPath->waypoints[selectedVertex];
    buffer.pushText(string_format("%.2f,%.2f", v.x, v.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
}


i32 PathTool::selectVertex(Vector2& pos) {
    Vector2 vertex;
    for(i32 i = 0; i < selectedPath->waypoints.size(); ++i) {
        vertex = selectedPath->waypoints[i];
        auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                              vertex.x + 5, vertex.y + 5);
        rect.round();
        if(rect.containsPoint(pos.x, pos.y)) {
            return i;
        }
    }
    return -1;
}

Entity *PathTool::getNewSelected() {
    return nullptr;
}

void PathTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffer.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void PathTool::renderInfo() {
    if(selected) {
        if(selectedPath) {
            if (!makingNew) {
                renderTextRight("Select and drag vertices with the mouse", 1);
                renderTextRight("Insert vertex [INS]", 3);
                renderTextRight("Delete vertex [DEL]", 4);
            } else {
                renderTextRight("Click anywhere to add a new vertex to the path", 1);
                renderTextRight("Delete last vertex [DEL]", 3);
                renderTextRight("Finish path [ENTER]", 4);
            }
        } else {
            renderTextRight("Click anywhere to start a new path", 1);
        }
    } else {
        renderTextRight("Select and object to edit or create its path", 1);
    }


}


