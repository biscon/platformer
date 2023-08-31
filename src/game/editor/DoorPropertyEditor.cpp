//
// Created by bison on 05-02-23.
//

#include <imgui.h>
#include "imgui_stdlib.h"
#include "DoorPropertyEditor.h"
#include "../components/DoorComponent.h"

void DoorPropertyEditor::show() {
    RectanglePropertyEditor::show();

    auto door = selected->get<DoorComponent>();

    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Door Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

    ImGui::InputText("Spawn Id", &door->spawnId);
    ImGui::InputText("Level File Name", &door->levelFileName);

    ImGui::PopStyleVar();
    ImGui::End();
}

void DoorPropertyEditor::onResizeComplete() {

}

void DoorPropertyEditor::onResize() {
    auto door = selected->get<DoorComponent>();
    door->rect = *rect;
}

void DoorPropertyEditor::onMove(const Vector2 &newPos, const Vector2& delta) {
    auto door = selected->get<DoorComponent>();
    float w = door->rect.width();
    float h = door->rect.height();
    door->rect.left = newPos.x;
    door->rect.top = newPos.y;
    door->rect.right = newPos.x + w;
    door->rect.bottom = newPos.y + h;
}

void DoorPropertyEditor::buildRect() {
    if(selected) {
        auto door = selected->get<DoorComponent>();
        if (door.isValid()) {
            if (rect) {
                *rect = door->rect;
            } else {
                rect = std::make_unique<FloatRect>(door->rect);
            }
        } else {
            rect = nullptr;
        }
    } else {
        rect = nullptr;
    }
}
