//
// Created by bison on 22-01-23.
//

#include <imgui.h>
#include <SDL_log.h>
#include "TerrainPropertyEditor.h"
#include "../components/TerrainComponent.h"
#include "ToolUtil.h"

void TerrainPropertyEditor::show() {
    propertyWindow();
    if(selected) {
        if(editing) {
            renderEditTerrain();
            editWindow();
            if(movingVertex) {
                moveVertex();
            }
        } else {
            renderSelection();
        }
    }

}

void TerrainPropertyEditor::propertyWindow() {
    auto terrain = selected->get<TerrainComponent>();
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Terrain Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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
    ImGui::Begin("Edit Terrain", &editing, ImGuiWindowFlags_AlwaysAutoResize);
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

void TerrainPropertyEditor::renderEditTerrain() {
    Vector2 camOffset(-camera.scrollX, -camera.scrollY);
    auto terrain = selected->get<TerrainComponent>();
    if(terrain.isValid()) {
        Vector2 lineStart, lineEnd;
        for(i32 i = 0; i < terrain->polygon.size(); ++i) {
            lineStart = terrain->polygon[i];
            lineStart += camOffset;
            if (i == terrain->polygon.size() - 1) {
                lineEnd = terrain->polygon[0];
                lineEnd += camOffset;

                if(false) {
                    buffers.unlit.pushLine(lineStart, lineEnd, LAST_SEGMENT_COLOR);
                    renderVertex(lineStart, GREEN);
                } else {
                    buffers.unlit.pushLine(lineStart, lineEnd, WHITE);
                    renderVertex(lineStart, selectedVertex == i ? GREEN : YELLOW);
                }
                break;
            } else {
                lineEnd = terrain->polygon[i + 1];
                lineEnd += camOffset;
            }
            buffers.unlit.pushLine(lineStart, lineEnd, WHITE);
            renderVertex(lineStart, selectedVertex == i ? GREEN : YELLOW);
        }
    }
}

void TerrainPropertyEditor::renderVertex(Vector2& vertex, const Color& color) {
    auto rect = FloatRect(vertex.x - vertexHandleSize, vertex.y - vertexHandleSize,
                          vertex.x + vertexHandleSize, vertex.y + vertexHandleSize);
    rect.round();
    buffers.unlit.pushRect(rect, color);
}


bool TerrainPropertyEditor::onLeftDown(Vector2 pos) {
    if(editing) {
        if(selectedVertex == -1) {
            selectedVertex = selectVertex(pos);
            if (selectedVertex != -1) {
                SDL_Log("Selected terrain vertex index = %d", selectedVertex);
            }
        }
        if(selectedVertex != -1) {
            auto terrain = selected->get<TerrainComponent>();
            auto vertex = terrain->polygon[selectedVertex];
            auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                                  vertex.x + 5, vertex.y + 5);
            rect.round();
            if(rect.containsPoint(pos.x, pos.y)) {
                startMovingVertex(pos, vertex, terrain.get());
                return true;
            } else {
                selectedVertex = selectVertex(pos);
                if(selectedVertex != -1) {
                    startMovingVertex(pos, terrain->polygon[selectedVertex], terrain.get());
                    return true;
                }
            }
        }
    }
    return false;
}

bool TerrainPropertyEditor::onLeftUp(Vector2 pos) {
    if(movingVertex) {
        movingVertex = false;
        auto terrain = selected->get<TerrainComponent>();
        if(terrain.isValid()) {
            SDL_Log("stop moving vertex");
            terrain->rebuildMesh();
        }
        return true;
    }
    return false;
}

bool TerrainPropertyEditor::onAction(const Action &action) {
    return false;
}

void TerrainPropertyEditor::startMovingVertex(Vector2& pos, Vector2 vertex, TerrainComponent& terrain) {
    SDL_Log("start move vertex");
    movingVertex = true;
    moveStartVertex = pos;
    snapToGrid(moveStartVertex);
    snapToGrid(vertex);
    orgVertex = vertex;
    terrain.polygon[selectedVertex] = vertex;
    terrain.rebuildMesh();
}

void TerrainPropertyEditor::moveVertex() {
    auto &mouse = inputDevice.getMouseState();
    auto terrain = selected->get<TerrainComponent>();
    if(terrain.isValid()) {
        Vector2 mouseDelta(mouse.x + camera.scrollX, mouse.y + camera.scrollY);
        snapToGrid(mouseDelta);
        mouseDelta -= moveStartVertex;
        if(mouseDelta.x != 0 || mouseDelta.y != 0) {
            terrain->polygon[selectedVertex] = orgVertex;
            //SDL_Log("Moving dx,dy = %.2f,%.2f", mouseDelta.x, mouseDelta.y);
            terrain->polygon[selectedVertex] += mouseDelta;
            terrain->rebuildMesh();
        }
        auto v = terrain->polygon[selectedVertex];
        buffers.unlit.pushText(string_format("%.2f,%.2f", v.x, v.y), &font, roundf(mouse.x + 10), roundf(mouse.y), WHITE);
    }
}


i32 TerrainPropertyEditor::selectVertex(Vector2& pos) {
    auto terrain = selected->get<TerrainComponent>();
    if(terrain.isValid()) {
        Vector2 vertex;
        for(i32 i = 0; i < terrain->polygon.size(); ++i) {
            vertex = terrain->polygon[i];
            auto rect = FloatRect(vertex.x - 5, vertex.y - 5,
                                  vertex.x + 5, vertex.y + 5);
            rect.round();
            if(rect.containsPoint(pos.x, pos.y)) {
                return i;
            }
        }
    }
    return -1;
}

