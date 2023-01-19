//
// Created by bison on 24-12-22.
//

#include <SDL_log.h>
#include "LadderTool.h"
#include "../components/LadderComponent.h"

void LadderTool::onUpdate() {
    RectangleTool::onUpdate();
    renderInfo();
}

void LadderTool::onRectComplete() {
    Entity* ladder = world->create();
    ladder->assign<LadderComponent>(*rect);
    setSelected(ladder);
    newSelected = ladder;
}

Entity *LadderTool::getNewSelected() {
    auto tmp = newSelected;
    newSelected = nullptr;
    return tmp;
}

void LadderTool::setSelected(Entity *ent) {
    /*
    if(ent == selected) {
        return;
    }
     */
    SDL_Log("setting new selected in laddertool");
    selected = ent;
    buildRect();
}

void LadderTool::onResizeComplete() {

}

void LadderTool::onResize() {
    auto ladder = selected->get<LadderComponent>();
    if(ladder.isValid()) {
        ladder->rect = *rect;
    }
}

void LadderTool::reset() {
    RectangleTool::reset();
    buildRect();
}

void LadderTool::buildRect() {
    if(selected) {
        auto ladder = selected->get<LadderComponent>();
        if (ladder.isValid()) {
            if (rect) {
                *rect = ladder->rect;
            } else {
                rect = std::make_unique<FloatRect>(ladder->rect);
            }
        } else {
            rect = nullptr;
        }
    } else {
        rect = nullptr;
    }
}

void LadderTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffer.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void LadderTool::renderInfo() {
    if(selected) {
        renderTextRight("Resize with mouse", 1);
        renderTextRight("Delete ladder [DEL]", 3);
    } else {
        renderTextRight("Click & drag to draw a new ladder", 1);
    }
}
