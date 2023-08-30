//
// Created by bison on 22-01-23.
//

#include <imgui.h>
#include <SDL_log.h>
#include "WindEffectPropertyEditor.h"
#include "../components/WindEffectComponent.h"

void WindEffectPropertyEditor::show() {
    auto wind = selected->get<WindEffectComponent>();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Wind Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

    ImGui::InputFloat("Speed", &wind->speed, 0.0f, 100.0f);
    ImGui::SliderFloat("Min Strength", &wind->minStrength, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Max Strength", &wind->maxStrength, 0.0f, 1.0f, "%.2f");
    ImGui::InputFloat("Strength Scale", &wind->strengthScale);
    ImGui::InputFloat("Interval", &wind->interval);
    ImGui::InputFloat("Detail", &wind->detail);
    ImGui::SliderFloat("Distortion", &wind->distortion, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Height Offset", &wind->heightOffset, 0.0f, 1.0f, "%.2f");

    ImGui::PopStyleVar();
    ImGui::End();
}

void WindEffectPropertyEditor::setSelected(Entity *ent) {
    selected = ent;
}

bool WindEffectPropertyEditor::onLeftDown(Vector2 pos) {
    return false;
}

bool WindEffectPropertyEditor::onLeftUp(Vector2 pos) {
    return false;
}

bool WindEffectPropertyEditor::onAction(const Input::Action &action) {
    return false;
}
