//
// Created by bison on 22-01-23.
//

#include <imgui.h>
#include <SDL_log.h>
#include "TransformPropertyEditor.h"
#include "../components/TransformComponent.h"

void TransformPropertyEditor::show() {
    auto transform = selected->get<TransformComponent>();
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
}

void TransformPropertyEditor::setSelected(Entity *ent) {
    selected = ent;
}
