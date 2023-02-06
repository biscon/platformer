//
// Created by bison on 05-02-23.
//

#include "LadderPropertyEditor.h"
#include "../components/LadderComponent.h"

void LadderPropertyEditor::show() {
    RectanglePropertyEditor::show();

}

void LadderPropertyEditor::onResizeComplete() {

}

void LadderPropertyEditor::onResize() {
    auto ladder = selected->get<LadderComponent>();
    ladder->rect = *rect;
}

void LadderPropertyEditor::onMove(const Vector2 &newPos, const Vector2& delta) {
    auto ladder = selected->get<LadderComponent>();
    float w = ladder->rect.width();
    float h = ladder->rect.height();
    ladder->rect.left = newPos.x;
    ladder->rect.top = newPos.y;
    ladder->rect.right = newPos.x + w;
    ladder->rect.bottom = newPos.y + h;
}

void LadderPropertyEditor::buildRect() {
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
