//
// Created by bison on 24-12-22.
//

#include <SDL_log.h>
#include "PlatformTool.h"
#include "../components/CollisionComponent.h"
#include "../Utils.h"
#include "../components/PlatformComponent.h"
#include "../components/TransformComponent.h"

void PlatformTool::onUpdate() {
    RectangleTool::onUpdate();
    renderInfo();
}

void PlatformTool::onRectComplete() {
    Entity* platform = world->create();
    platform->assign<TransformComponent>(Vector2(rect->left, rect->top), 1.0f, 0, 0);
    platform->assign<TerrainComponent>();
    platform->assign<PlatformComponent>(Vector2(rect->width(), rect->height()), 200.0f);
    platform->assign<CollisionComponent>();
    rebuildMovingPlatform(platform);

    setSelected(platform);
    newSelected = platform;
}

Entity *PlatformTool::getNewSelected() {
    auto tmp = newSelected;
    newSelected = nullptr;
    return tmp;
}

void PlatformTool::setSelected(Entity *ent) {
    /*
    if(ent == selected) {
        return;
    }
     */
    SDL_Log("setting new selected in platformtool");
    selected = ent;
    buildRect();
}

void PlatformTool::onResizeComplete() {

}

void PlatformTool::onResize() {
    auto transform = selected->get<TransformComponent>();
    auto platform = selected->get<PlatformComponent>();
    if(transform.isValid() && platform.isValid()) {
        transform->pos.x = rect->left;
        transform->pos.y = rect->top;
        platform->size.x = rect->width();
        platform->size.y = rect->height();
        rebuildMovingPlatform(selected);
    }
}

void PlatformTool::reset() {
    RectangleTool::reset();
    buildRect();
}

void PlatformTool::buildRect() {
    if(selected) {
        auto platform = selected->get<PlatformComponent>();
        auto transform = selected->get<TransformComponent>();
        if (platform.isValid() && transform.isValid()) {
            FloatRect newRect(transform->pos.x, transform->pos.y, transform->pos.x + platform->size.x, transform->pos.y + platform->size.y);
            if (rect) {
                *rect = newRect;
            } else {
                rect = std::make_unique<FloatRect>(newRect);
            }
        } else {
            rect = nullptr;
        }
    } else {
        rect = nullptr;
    }
}

void PlatformTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffer.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void PlatformTool::renderInfo() {
    if(selected) {
        renderTextRight("Resize with mouse", 1);
        renderTextRight("Delete platform [DEL]", 3);
    } else {
        renderTextRight("Click & drag to draw a new platform", 1);
    }
}
