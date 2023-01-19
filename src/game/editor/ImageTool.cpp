//
// Created by bison on 26-12-22.
//

#include <SDL_log.h>
#include "ImageTool.h"
#include "../components/ImageComponent.h"
#include "../../util/string_util.h"
#include "../components/TransformComponent.h"

void ImageTool::onUpdate() {
    RectangleTool::onUpdate();
    renderInfo();
}

void ImageTool::onRectComplete() {
}

Entity *ImageTool::getNewSelected() {
    return nullptr;
}

void ImageTool::setSelected(Entity *ent) {
    selected = ent;
    buildRect();
}

void ImageTool::onResizeComplete() {

}

void ImageTool::onResize() {
    auto image = selected->get<ImageComponent>();
    auto transform = selected->get<TransformComponent>();
    if(image.isValid() && transform.isValid()) {
        image->width = rect->width() / transform->scale;
        image->height = rect->height() / transform->scale;
    }
}

void ImageTool::reset() {
    RectangleTool::reset();
    buildRect();
}

void ImageTool::buildRect() {
    if(selected) {
        auto image = selected->get<ImageComponent>();
        auto transform = selected->get<TransformComponent>();
        if (image.isValid() && transform.isValid()) {
            FloatRect imageRect;
            imageRect.left = transform->pos.x;
            imageRect.top = transform->pos.y;
            imageRect.right = transform->pos.x + (transform->scale * image->width);
            imageRect.bottom = transform->pos.y + (transform->scale * image->height);
            if (rect) {
                *rect = imageRect;
            } else {
                rect = std::make_unique<FloatRect>(imageRect);
            }
        } else {
            rect = nullptr;
        }
    } else {
        rect = nullptr;
    }
}

void ImageTool::renderTextRight(const std::string& text, i32 line) {
    float len = font.measureTextWidth(text);
    float margin = 10.0f;
    float lineSpacing = 5.0f;
    buffer.pushText(text, &font, 1920 - len - margin, (line * (font.getSize() + lineSpacing)) + margin, WHITE);
}

void ImageTool::renderInfo() {
    if(selected) {
        renderTextRight("Resize with mouse", 1);
        renderTextRight("Delete image [DEL]", 3);
        auto transform = selected->get<TransformComponent>();
        if(transform.isValid()) {
            renderTextRight(string_format("Layer %d [UP/DOWN]", transform->layer), 4);
        }
    } else {
        renderTextRight("Select a image object", 1);
    }
}

bool ImageTool::onKeyboard(MappedId key) {
    auto transform = selected->get<TransformComponent>();
    if(key == INPUT_ACTION_UP && transform.isValid()) {
        transform->layer += 1;
        return true;
    }
    if(key == INPUT_ACTION_DOWN && transform.isValid()) {
        transform->layer -= 1;
        return true;
    }
    return false;
}
