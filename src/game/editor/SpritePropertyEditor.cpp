//
// Created by bison on 05-02-23.
//

#include <imgui.h>
#include "SpritePropertyEditor.h"
#include "../components/TransformComponent.h"
#include "../components/SpriteComponent.h"
#include "../Utils.h"

void SpritePropertyEditor::show() {
    auto sprite = selected->get<SpriteComponent>();
    RectanglePropertyEditor::show();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Sprite Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

    ImGui::SliderFloat("Alpha", &sprite->alpha, 0.0, 1.0f, "%.2f");
    ImGui::Checkbox("HorizFlip", &sprite->horizFlip);
    ImGui::Checkbox("VertFlip", &sprite->vertFlip);
    if(ImGui::Button("Animations")) {
        showAnimations = true;
    }

    ImGui::PopStyleVar();
    ImGui::End();

    if(showAnimations) {
        animationEditor();
    }
}

void SpritePropertyEditor::animationEditor() {
    auto sprite = selected->get<SpriteComponent>();

    ImGui::SetNextWindowSize(ImVec2(750,450), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Animations##SpritePropertyEditor", &showAnimations);

    auto& animations = sprite->getAnimations();
    static u32 selected = 0;
    ImGui::BeginGroup();
    ImGui::BeginChild("left pane", ImVec2(120, -ImGui::GetFrameHeightWithSpacing()), true);
    for (u32 i = 0; i < animations.size(); i++) {
        std::string id = animations[i].name;
        if(id.empty()) {
            id = string_format("Animation %d", i);
        }
        if (ImGui::Selectable(id.c_str(), selected == i))
            selected = i;

    }
    ImGui::EndChild();
    if (ImGui::Button("+", ImVec2(25, 0))) {
        if((selected >= 0 && selected < animations.size()) || animations.empty()) {


            //animations.insert(animations.begin() + selected, spawn);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("-", ImVec2(25, 0))) {
        if(selected >= 0 && selected < animations.size()) {
            animations.erase(animations.begin() + selected);
        }
    }
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::BeginGroup();
    if(!animations.empty()) {
        ImGui::BeginChild("item view", ImVec2(0,
                                              -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        auto &sp = animations[selected];
        //ImGui::InputText("id", &sp.id);

        ImGui::EndChild();
    }
    ImGui::EndGroup();



    ImGui::End();
}

void SpritePropertyEditor::onResizeComplete() {

}

void SpritePropertyEditor::onResize() {

}

void SpritePropertyEditor::onMove(const Vector2 &newPos, const Vector2& delta) {
    auto transform = selected->get<TransformComponent>();
    auto sprite = selected->get<SpriteComponent>();
    if(transform.isValid()) {
        Vector2 origin;
        sprite->getOrigin(&origin.x, &origin.y, transform->scale);
        transform->pos = newPos + origin;
        buildRect();
    }
}

void SpritePropertyEditor::buildRect() {
    if(selected) {
        auto sprite = selected->get<SpriteComponent>();
        auto transform = selected->get<TransformComponent>();
        if (sprite.isValid() && transform.isValid()) {
            FloatRect imageRect;
            getBounds(selected, imageRect);
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
