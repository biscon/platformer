//
// Created by bison on 29-01-23.
//

#include "ImagePropertyEditor.h"
#include "../components/ImageComponent.h"
#include <imgui.h>
#include <SDL_log.h>

void ImagePropertyEditor::show() {
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

void ImagePropertyEditor::setSelected(Entity *ent) {
    selected = ent;
}

bool ImagePropertyEditor::onLeftDown(Vector2 pos) {
    return false;
}

bool ImagePropertyEditor::onLeftUp(Vector2 pos) {
    return false;
}

bool ImagePropertyEditor::onAction(const Input::Action &action) {
    return false;
}
