//
// Created by bison on 20-12-22.
//

#include <SDL_log.h>
#include "Editor.h"
#include "../components/TerrainComponent.h"
#include "../PolyUtil.h"
#include "../../util/string_util.h"
#include "MoveTool.h"
#include "TerrainTool.h"
#include "RectangleTool.h"
#include "LadderTool.h"
#include "../components/ImageComponent.h"
#include "ImageTool.h"
#include "ToolUtil.h"
#include "PlatformTool.h"
#include "../components/CollisionComponent.h"
#include "../Utils.h"
#include "PathTool.h"
#include "LightTool.h"
#include "../components/PointLightComponent.h"
#include "../components/PlatformComponent.h"
#include "../components/TransformComponent.h"
#include "../components/PathComponent.h"
#include "../components/LadderComponent.h"
#include "imgui.h"
#include "../components/FlickerEffectComponent.h"
#include "../components/GlowEffectComponent.h"
#include "TransformPropertyEditor.h"
#include "TerrainPropertyEditor.h"


Editor::Editor(IInputDevice &inputDevice, World* world, Camera& camera, RenderBuffers buffers, Font &font) :
        inputDevice(inputDevice), world(world), camera(camera), buffers(buffers), font(font) {
    inputContext = std::make_shared<Input::InputContext>();
    inputDevice.registerContext(inputContext);
    inputContext->registerAction(INPUT_ACTION_LMB_DOWN);
    inputContext->registerAction(INPUT_ACTION_LMB_UP);
    inputContext->registerAction(INPUT_ACTION_DEL);
    inputContext->registerAction(INPUT_ACTION_INS);
    inputContext->registerAction(INPUT_ACTION_ENTER);
    inputContext->registerAction(INPUT_ACTION_DUPLICATE);
    inputContext->registerAction(INPUT_ACTION_HIDE);
    inputContext->registerAction(INPUT_ACTION_GRID);

    inputContext->registerAction(INPUT_ACTION_TOOL_1);
    inputContext->registerAction(INPUT_ACTION_TOOL_2);
    inputContext->registerAction(INPUT_ACTION_TOOL_3);
    inputContext->registerAction(INPUT_ACTION_TOOL_4);
    inputContext->registerAction(INPUT_ACTION_TOOL_5);
    inputContext->registerAction(INPUT_ACTION_TOOL_6);

    inputContext->registerAction(INPUT_ACTION_UP);
    inputContext->registerAction(INPUT_ACTION_DOWN);

    propertyEditorMap[ComponentType::Transform] = std::make_unique<TransformPropertyEditor>();
    propertyEditorMap[ComponentType::Terrain] = std::make_unique<TerrainPropertyEditor>(inputDevice, buffers, font, camera, world);
}

void Editor::update(float deltaTime) {
    bool ignoreClick = false;

    // dispatch input
    Input::Action action = {};
    auto &mouse = inputDevice.getMouseState();
    while(inputContext->pollAction(action)) {

        switch(action.id) {
            case INPUT_ACTION_LMB_DOWN: {
                if(!ignoreClick) {
                    onLeftDown(mouse.x, mouse.y);
                }
                break;
            }
            case INPUT_ACTION_LMB_UP: {
                if(!ignoreClick) {
                    onLeftUp(mouse.x, mouse.y);
                }
                break;
            }
            case INPUT_ACTION_TOOL_1:
            case INPUT_ACTION_TOOL_2:
            case INPUT_ACTION_TOOL_3:
            case INPUT_ACTION_TOOL_4:
            case INPUT_ACTION_TOOL_5:
            case INPUT_ACTION_TOOL_6:{
                break;
            }
            default: {
                break;
            }
        }
    }

    float margin = 8.0f;
    FloatRect levelRect(-camera.scrollX, -camera.scrollY, -camera.scrollX + camera.levelWidth, -camera.scrollY + camera.levelHeight);
    buffers.unlit.pushRect(levelRect, RED);
    buffers.unlit.pushText(string_format("%.2f,%.2f", 0.0f, 0.0f), &font, levelRect.left, levelRect.top - margin, WHITE);
    std::string text = string_format("%.2f,%.2f", camera.levelWidth, 0.0f);
    buffers.unlit.pushText(text, &font, levelRect.right - font.measureTextWidth(text), levelRect.top - margin, WHITE);
    text = string_format("%.2f,%.2f", 0.0f, camera.levelHeight);
    buffers.unlit.pushText(text, &font, levelRect.left, levelRect.bottom + margin + font.getSize(), WHITE);

    text = string_format("%.2f,%.2f", camera.levelWidth, camera.levelHeight);
    buffers.unlit.pushText(text, &font, levelRect.right - font.measureTextWidth(text), levelRect.bottom + margin + font.getSize(), WHITE);

    text = string_format("GridSize: %d", getGridSizes()[currentGridSize]);
    buffers.unlit.pushText(text, &font, 1920.0f - font.measureTextWidth(text) - margin, 1080.0f - margin, WHITE);

    updateMetaData();
    showEntityComponentSelector();
    showComponentProperties();
}


void Editor::cycleGridSize() {
    currentGridSize++;
    auto &gridSizes = getGridSizes();
    if(currentGridSize > gridSizes.size() - 1) {
        currentGridSize = 0;
    }
    setGridSize(currentGridSize);
}

void Editor::reset() {
    setSelected(nullptr);
    selectedTool = 0;

    selectedComponent = ComponentType::None;
    selectedEntity = nullptr;
    resetEditors();
    metaData.clear();
}

void Editor::resetEditors() {
    for(auto& pair : propertyEditorMap) {
        pair.second->setSelected(nullptr);
    }
}

void Editor::setSelected(Entity *ent) {
    selected = ent;
}

void Editor::duplicate(Entity *ent) {
    auto newEntity = world->create();
    auto oldTerrain = ent->get<TerrainComponent>();
    auto oldTransform = ent->get<TransformComponent>();
    auto oldPlatform = ent->get<PlatformComponent>();
    auto oldPath = ent->get<PathComponent>();
    if(oldPlatform.isValid()) {
        newEntity->assign<TransformComponent>(oldTransform.get());
        newEntity->assign<TerrainComponent>();
        newEntity->assign<PlatformComponent>(oldPlatform.get());
        newEntity->assign<CollisionComponent>();
        rebuildMovingPlatform(newEntity);
    } else if(oldTerrain.isValid()) {
        newEntity->assign<TerrainComponent>(oldTerrain->polygon, oldTerrain->show);
        auto newTerrain = newEntity->get<TerrainComponent>();
        newTerrain->rebuildMesh();
    }
    auto oldLadder = ent->get<LadderComponent>();
    if(oldLadder.isValid()) {
        newEntity->assign<LadderComponent>(oldLadder->rect);
    }
    auto oldImage = ent->get<ImageComponent>();
    if(oldImage.isValid() && oldTransform.isValid()) {
        newEntity->assign<TransformComponent>(oldTransform.get());
        newEntity->assign<ImageComponent>(oldImage.get());
    }
    if(oldPath.isValid()) {
        newEntity->assign<PathComponent>(oldPath.get());
    }
    setSelected(newEntity);
}

void Editor::onDelete() {
    if(selected) {
        world->destroy(selected, true);
        setSelected(nullptr);
    }
}

void Editor::onLeftDown(float x, float y) {
    Vector2 pos(x + camera.scrollX, y + camera.scrollY);
    SDL_Log("onLeftDown %.2f,%.2f", pos.x, pos.y);

    bool clickedNothing = true;
    for (Entity *ent : world->all(false)) {
        auto terrain = ent->get<TerrainComponent>();
        auto ladder = ent->get<LadderComponent>();
        auto image = ent->get<ImageComponent>();
        auto transform = ent->get<TransformComponent>();
        auto light = ent->get<PointLightComponent>();
        if(image.isValid() && transform.isValid()) {
            FloatRect imageRect;
            imageRect.left = transform->pos.x;
            imageRect.top = transform->pos.y;
            imageRect.right = transform->pos.x + (transform->scale * image->width);
            imageRect.bottom = transform->pos.y + (transform->scale * image->height);
            if(imageRect.containsPoint(pos.x, pos.y)) {
                clickedNothing = false;
                if(ent != selected) {
                    setSelected(ent);
                    break;
                }
            }
        }
        if(terrain.isValid()) {
            if(pointInPolygon(pos, terrain->polygon)) {
                clickedNothing = false;
                if(ent != selected) {
                    setSelected(ent);
                    break;
                }
            }
        }
        if(ladder.isValid()) {
            if(ladder->rect.containsPoint(pos.x, pos.y)) {
                clickedNothing = false;
                if(ent != selected) {
                    setSelected(ent);
                    break;
                }
            }
        }
        if(light.isValid() && transform.isValid()) {
            if(pointInCircle(pos, transform->pos, light->outerRadius)) {
                clickedNothing = false;
                if(ent != selected) {
                    setSelected(ent);
                    break;
                }
            }
        }
    }
    if(clickedNothing) {
        SDL_Log("clicked nothing");
        setSelected(nullptr);
    }
}

void Editor::onLeftUp(float x, float y) {
    Vector2 pos(x + camera.scrollX, y + camera.scrollY);
    SDL_Log("onLeftUp %.2f,%.2f", pos.x, pos.y);
}

void Editor::showEntityComponentSelector() {
    ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Entities"))
    {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));
    ImGui::Columns(1);
    //ImGui::Separator();

    for (Entity *entity : world->all(false)) {
        auto entityId = entity->getEntityId();
        ImGui::PushID((i32) entityId);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
        ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.

        bool node_open = ImGui::TreeNode(metaData[entityId].name.c_str());
        if (node_open)
        {
            for(auto& pair : metaData[entityId].componentTypes) {
                auto type = pair.first;
                ImGui::PushID(&pair.first); // Use field index as identifier.
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                ImGui::AlignTextToFramePadding();
                bool isSelected = false;
                if(selectedEntity == entity && selectedComponent == type) {
                    isSelected = true;
                }
                if(ImGui::Selectable(getComponentName(type).c_str(), isSelected)) {
                    selectedEntity = entity;
                    selectedComponent = type;
                }
                if (ImGui::BeginPopupContextItem("componentContextMenu"))
                {
                    if (ImGui::Selectable("Delete")) {
                        SDL_Log("Clicked delete");
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    ImGui::Columns(1);
    //ImGui::Separator();
    ImGui::PopStyleVar();
    ImGui::End();

    ImGui::ShowDemoWindow();
}

void Editor::showComponentProperties() {
    if(selectedComponent == ComponentType::None || selectedEntity == nullptr) {
        return;
    }
    if(propertyEditorMap.count(selectedComponent) > 0) {
        propertyEditorMap[selectedComponent]->setSelected(selectedEntity);
        propertyEditorMap[selectedComponent]->show();
    }
}

static void insertComponentIfNotExist(std::map<ComponentType, bool> &componentTypes, ComponentType type) {
    if(componentTypes.count(type) == 0) {
        componentTypes[type] = false;
    }
}

void Editor::updateMetaData() {
    //metaData.clear();
    for (Entity *ent : world->all(false)) {

        if(metaData.count(ent->getEntityId()) == 0) {
            EntityMetaData entityMetaData;
            entityMetaData.entity = ent;
            entityMetaData.name = string_format("Entity %zu", ent->getEntityId());
            metaData[ent->getEntityId()] = entityMetaData;
        }
        EntityMetaData& entityMetaData = metaData[ent->getEntityId()];
        if(ent->has<TransformComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Transform);
        if(ent->has<TerrainComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Terrain);
        if(ent->has<LadderComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Ladder);
        if(ent->has<ImageComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Image);
        if(ent->has<PlatformComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Platform);
        if(ent->has<PathComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Path);
        if(ent->has<CollisionComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::Collision);
        if(ent->has<PointLightComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::PointLight);
        if(ent->has<FlickerEffectComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::FlickerEffect);
        if(ent->has<GlowEffectComponent>()) insertComponentIfNotExist(entityMetaData.componentTypes, ComponentType::GlowEffect);
    }
}

std::string getComponentName(ComponentType type) {
    switch(type) {
        case ComponentType::Transform: return "Transform";
        case ComponentType::Terrain: return "Terrain";
        case ComponentType::Ladder: return "Ladder";
        case ComponentType::Image: return "Image";
        case ComponentType::Platform: return "Platform";
        case ComponentType::Path: return "Path";
        case ComponentType::Collision: return "Collision";
        case ComponentType::PointLight: return "PointLight";
        case ComponentType::FlickerEffect: return "FlickerEffect";
        case ComponentType::GlowEffect: return "GlowEffect";
    }
    return "Unknown";
}


