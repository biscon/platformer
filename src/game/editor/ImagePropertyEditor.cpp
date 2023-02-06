//
// Created by bison on 29-01-23.
//

#include "ImagePropertyEditor.h"
#include "../components/ImageComponent.h"
#include "../components/TransformComponent.h"
#include <imgui.h>
#include <SDL_log.h>

void ImagePropertyEditor::show() {
    RectanglePropertyEditor::show();

    auto image = selected->get<ImageComponent>();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Image Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

    ImGui::InputFloat("Width", &image->width, 0, 0, "%.0f");
    ImGui::InputFloat("Height", &image->height, 0, 0, "%.0f");
    ImGui::SliderFloat("Alpha", &image->alpha, 0.0, 1.0f, "%.2f");
    ImGui::Checkbox("LightMap", &image->lightMap);

    ImGui::PopStyleVar();
    ImGui::End();

}

void ImagePropertyEditor::buildRect() {
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

void ImagePropertyEditor::onResizeComplete() {

}

void ImagePropertyEditor::onResize() {
    auto image = selected->get<ImageComponent>();
    auto transform = selected->get<TransformComponent>();
    if(image.isValid() && transform.isValid()) {
        image->width = rect->width() / transform->scale;
        image->height = rect->height() / transform->scale;
    }
}

void ImagePropertyEditor::onMove(const Vector2 &newPos, const Vector2& delta) {
    auto transform = selected->get<TransformComponent>();
    if(transform.isValid()) {
        transform->pos = newPos;
        buildRect();
    }
}
