//
// Created by bison on 22-01-23.
//

#include <imgui.h>
#include "TerrainPropertyEditor.h"
#include "../components/TerrainComponent.h"

void TerrainPropertyEditor::show() {
    auto terrain = selected->get<TerrainComponent>();
    ImGui::Checkbox("Show", &terrain->show);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip("Render terrain during gameplay.");
    ImGui::Checkbox("Through", &terrain->through);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip("Allow the player to jump through or fall down from terrain.");
    ImGui::Checkbox("Light", &terrain->light);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip("Is the terrain affected by lighting calculations.");
    ImGui::ColorEdit4("Color", (float*) &terrain->color);
}

void TerrainPropertyEditor::setSelected(Entity *ent) {
    selected = ent;
}
