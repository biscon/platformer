//
// Created by bison on 22-01-23.
//

#include <imgui.h>
#include <SDL_log.h>
#include "TerrainPropertyEditor.h"
#include "../components/TerrainComponent.h"

void TerrainPropertyEditor::show() {
    propertyWindow();
    if(selected) {
        renderSelection();
    }
    if(editing) {
        editWindow();
    }
}

void TerrainPropertyEditor::propertyWindow() {
    auto terrain = selected->get<TerrainComponent>();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Terrain Properties", nullptr);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

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
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    //ImGui::Checkbox("Edit", &editing);
    if(ImGui::Button("Edit", ImVec2(100.0f, 0))) {
        editing = true;
    }
    ImGui::SameLine();
    if(ImGui::Button("Delete", ImVec2(100.0f, 0))) {
        SDL_Log("Delete Clicked");
        ImGui::OpenPopup("Delete?");
    }
    deletePrompt();

    ImGui::PopStyleVar();
    ImGui::End();
}

void TerrainPropertyEditor::editWindow() {
    auto terrain = selected->get<TerrainComponent>();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Edit Terrain", &editing);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));

    if(ImGui::Button("Insert vertex", ImVec2(120.0f, 0))) {
    }
    if(ImGui::Button("Delete vertex", ImVec2(120.0f, 0))) {
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void TerrainPropertyEditor::deletePrompt() {
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
        ImGui::Separator();

        //static int unused_i = 0;
        //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

        static bool dont_ask_me_next_time = false;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}


void TerrainPropertyEditor::setSelected(Entity *ent) {
    if(ent != selected) {
        editing = false;
    }
    selected = ent;
}

void TerrainPropertyEditor::renderSelection() {
    auto terrain = selected->get<TerrainComponent>();
    Vector2 camOffset(-camera.scrollX, -camera.scrollY);
    Vector2 lineStart, lineEnd;
    for(i32 i = 0; i < terrain->polygon.size(); ++i) {
        lineStart = terrain->polygon[i];
        lineStart += camOffset;
        if (i == terrain->polygon.size() - 1) {
            lineEnd = terrain->polygon[0];
            lineEnd += camOffset;
        } else {
            lineEnd = terrain->polygon[i + 1];
            lineEnd += camOffset;
        }
        buffers.unlit.pushLine(lineStart, lineEnd, WHITE);
    }
}
