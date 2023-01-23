//
// Created by bison on 22-01-23.
//

#include <imgui.h>
#include <SDL_log.h>
#include "TransformPropertyEditor.h"
#include "../components/TransformComponent.h"

void TransformPropertyEditor::show() {
    auto transform = selected->get<TransformComponent>();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Transform Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

    static float pos[2];
    pos[0] = transform->pos.x;
    pos[1] = transform->pos.y;
    ImGui::InputFloat2("Position", pos);
    transform->pos.x = pos[0];
    transform->pos.y = pos[1];

    ImGui::InputInt("Layer", &transform->layer);
    //ImGui::SliderInt("Layer", &transform->layer, -25, 25);
    ImGui::SliderFloat("Scale", &transform->scale, 0.001, 10.0f);
    //ImGui::InputFloat("Rotation", &transform->rotation, 0.0f, 360.0f);
    //ImGui::SliderAngle("Rotation", &transform->rotation);
    ImGui::SliderFloat("Rotation", &transform->rotation, 0.0f, 360.0f, "%.2f");

    ImGui::PopStyleVar();
    ImGui::End();
}

void TransformPropertyEditor::setSelected(Entity *ent) {
    selected = ent;
}

bool TransformPropertyEditor::onLeftDown(Vector2 pos) {
    return false;
}

bool TransformPropertyEditor::onLeftUp(Vector2 pos) {
    return false;
}

bool TransformPropertyEditor::onAction(const Input::Action &action) {
    return false;
}
