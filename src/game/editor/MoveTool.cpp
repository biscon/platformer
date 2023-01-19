//
// Created by bison on 21-12-22.
//

#include <SDL_log.h>
#include <Vector2.h>
#include "MoveTool.h"
#include "../components/TerrainComponent.h"
#include "ToolUtil.h"
#include "../../util/string_util.h"
#include "../PolyUtil.h"
#include "../components/ImageComponent.h"
#include "../Utils.h"
#include "../components/PointLightComponent.h"
#include "../components/PlatformComponent.h"
#include "../components/TransformComponent.h"
#include "../components/LadderComponent.h"

void MoveTool::onUpdate() {
    if(moving && selected) {
        move();
    }
    if(selected) {
        renderOutline();
    }
    renderInfo();
}

bool MoveTool::onKeyboard(MappedId key) {
    return false;
}

bool MoveTool::onLeftDown(Vector2 pos) {
    if (!selected) {
        return false;
    }

    auto image = selected->get<ImageComponent>();
    auto transform = selected->get<TransformComponent>();
    if (image.isValid() && transform.isValid()) {
        FloatRect imageRect;
        imageRect.left = transform->pos.x;
        imageRect.top = transform->pos.y;
        imageRect.right = transform->pos.x + (transform->scale * image->width);
        imageRect.bottom = transform->pos.y + (transform->scale * image->height);
        if (imageRect.containsPoint(pos.x, pos.y)) {
            if (!moving) {
                moving = true;
                moveStart = pos;
                snapToGrid(moveStart);
                snapToGrid(transform->pos);
                orgPos = transform->pos;
                SDL_Log("Start moving image");
                return true;
            }
        }
    }

    auto light = selected->get<PointLightComponent>();
    if(light.isValid() && transform.isValid()) {
        if(pointInCircle(pos, transform->pos, light->outerRadius)) {
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

    auto platform = selected->get<PlatformComponent>();
    auto terrain = selected->get<TerrainComponent>();
    if(platform.isValid() && transform.isValid() && terrain.isValid()) {
        FloatRect rect(transform->pos.x, transform->pos.y, transform->pos.x + platform->size.x, transform->pos.y + platform->size.y);
        if(rect.containsPoint(pos.x, pos.y)) {
            if(!moving) {
                moving = true;
                moveStart = pos;
                snapToGrid(moveStart);
                snapToGrid(transform->pos);
                orgPos = transform->pos;
                SDL_Log("Start moving platform");
                return true;
            }
        }
    } else if(terrain.isValid()) {
        if(pointInPolygon(pos, terrain->polygon)) {
            if(!moving) {
                moving = true;
                moveStart = pos;
                snapToGrid(moveStart);

                for (Vector2 &vertex : terrain->polygon) {
                    snapToGrid(vertex);
                }
                terrain->rebuildMesh();

                orgPoly = terrain->polygon;
                SDL_Log("Start moving terrain");
                return true;
            }
        }
    }

    auto ladder = selected->get<LadderComponent>();
    if(ladder.isValid()) {
        if(ladder->rect.containsPoint(pos.x, pos.y)) {
            if(!moving) {
                moving = true;
                moveStart = pos;
                snapToGrid(moveStart);
                snapToGrid(ladder->rect);
                orgRect = ladder->rect;
                SDL_Log("Start moving ladder");
                return true;
            }
        }
    }
    return false;
}

bool MoveTool::onLeftUp(Vector2 pos) {
    if(moving && selected) {
        moving = false;
        auto terrain = selected->get<TerrainComponent>();
        auto platform = selected->get<PlatformComponent>();
        if(platform.isValid()) {
            SDL_Log("stop moving platform");
            rebuildMovingPlatform(selected);
        } else if(terrain.isValid()) {
            SDL_Log("stop moving terrain");
            terrain->rebuildMesh();
        }
        return true;
    }
    return false;
}

void MoveTool::reset() {
    moving = false;
}

void MoveTool::setSelected(Entity *ent) {
    selected = ent;
}

void MoveTool::move() {
    auto &mouse = inputDevice.getMouseState();
    Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
    snapToGrid(mouseDelta);
    mouseDelta -= moveStart;
    auto terrain = selected->get<TerrainComponent>();
    auto ladder = selected->get<LadderComponent>();
    auto image = selected->get<ImageComponent>();
    auto transform = selected->get<TransformComponent>();
    auto platform = selected->get<PlatformComponent>();
    auto light = selected->get<PointLightComponent>();

    if(platform.isValid() && transform.isValid()) {
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            transform->pos = orgPos;
            transform->pos += mouseDelta;
            rebuildMovingPlatform(selected);
        }
    } else if(terrain.isValid()) {
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            terrain->polygon = orgPoly;
            for (Vector2 &vertex : terrain->polygon) {
                vertex += mouseDelta;
            }
            terrain->rebuildMesh();
        }
    }
    if(ladder.isValid()) {
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            ladder->rect = orgRect;
            ladder->rect.translate(mouseDelta.x, mouseDelta.y);
        }
    }
    if(image.isValid() && transform.isValid()) {
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            transform->pos = orgPos;
            transform->pos += mouseDelta;
        }
    }
    if(light.isValid() && transform.isValid()) {
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            transform->pos = orgPos;
            transform->pos += mouseDelta;
        }
    }
    buffer.pushText(string_format("+ %.2f,%.2f", mouseDelta.x, mouseDelta.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
}

void MoveTool::renderOutline() {
    Vector2 camOffset(-camera.scrollX, -camera.scrollY);
    auto terrain = selected->get<TerrainComponent>();
    auto ladder = selected->get<LadderComponent>();
    auto image = selected->get<ImageComponent>();
    auto transform = selected->get<TransformComponent>();
    auto platform = selected->get<PlatformComponent>();
    auto light = selected->get<PointLightComponent>();

    if(platform.isValid()) {
        FloatRect rect(transform->pos.x, transform->pos.y, transform->pos.x + platform->size.x, transform->pos.y + platform->size.y);
        rect.translate(camOffset.x, camOffset.y);
        buffer.pushRect(rect, WHITE);
    } else if(terrain.isValid()) {
        Vector2 lineStart, lineEnd;
        for(i32 i = 0; i < terrain->polygon.size(); ++i) {
            lineStart = terrain->polygon[i];
            lineStart += camOffset;
            if (i == terrain->polygon.size() - 1) {
                lineEnd = terrain->polygon[0];
                lineEnd += camOffset;
            } else {
                lineEnd = terrain->polygon[i + 1];
                lineEnd += camOffset;
            }
            buffer.pushLine(lineStart, lineEnd, WHITE);
        }
    }
    if(ladder.isValid()) {
        FloatRect rect = ladder->rect;
        rect.translate(camOffset.x, camOffset.y);
        buffer.pushRect(rect, WHITE);
    }
    if(image.isValid() && transform.isValid()) {
        FloatRect imageRect;
        imageRect.left = transform->pos.x;
        imageRect.top = transform->pos.y;
        imageRect.right = transform->pos.x + (transform->scale * image->width);
        imageRect.bottom = transform->pos.y + (transform->scale * image->height);
        imageRect.translate(camOffset.x, camOffset.y);
        buffer.pushRect(imageRect, WHITE);
    }
    if(light.isValid() && transform.isValid()) {
        auto pos = transform->pos;
        pos += camOffset;
        buffer.pushCircle(pos, light->outerRadius, light->segments, WHITE);
    }
}

Entity *MoveTool::getNewSelected() {
    return nullptr;
}

void MoveTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffer.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void MoveTool::renderInfo() {
    renderTextRight("Use mouse to select and drag objects", 1);
    renderTextRight("Delete object [DEL]", 3);
    renderTextRight("Duplicate object [C]", 4);
}

