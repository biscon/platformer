//
// Created by bison on 20-12-22.
//

#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd // stupid MSFT "deprecation" warning
#else
    #include <unistd.h>
#endif

#include <SDL_log.h>
#include <cstring>
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
#include "imgui_stdlib.h"
#include "../components/FlickerEffectComponent.h"
#include "../components/GlowEffectComponent.h"
#include "TransformPropertyEditor.h"
#include "TerrainPropertyEditor.h"
#include "../../util/ImGuiFileDialog.h"


Editor::Editor(IInputDevice &inputDevice, World* world, Camera& camera, RenderBuffers buffers, Font &font, Level& level) :
        inputDevice(inputDevice), world(world), camera(camera), buffers(buffers), font(font), level(level) {
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
    // dispatch input
    Input::Action action = {};
    auto &mouse = inputDevice.getMouseState();
    while(inputContext->pollAction(action)) {
        switch(action.id) {
            case INPUT_ACTION_LMB_DOWN: {
                onLeftDown(mouse.x, mouse.y);
                break;
            }
            case INPUT_ACTION_LMB_UP: {
                onLeftUp(mouse.x, mouse.y);
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

    text = string_format("GridSize: %d", currentGridSize);
    buffers.unlit.pushText(text, &font, 1920.0f - font.measureTextWidth(text) - margin, 1080.0f - margin, WHITE);

    updateMetaData();
    mainMenu();
    showEntityComponentSelector();
    showComponentProperties();
    if(showCreateEntityModal)
        createEntityModal();
    fileDialogs();
    if(showDemo) {
        ImGui::ShowDemoWindow();
    }
    if(showProperties) {
        properties();
    }
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
    if(selectedComponent == ComponentType::None || selectedEntity == nullptr) {
        return;
    }
    if(propertyEditorMap.count(selectedComponent) > 0) {
        propertyEditorMap[selectedComponent]->onLeftDown(pos);
    }
}

void Editor::onLeftUp(float x, float y) {
    Vector2 pos(x + camera.scrollX, y + camera.scrollY);
    SDL_Log("onLeftUp %.2f,%.2f", pos.x, pos.y);
    if(selectedComponent == ComponentType::None || selectedEntity == nullptr) {
        return;
    }
    if(propertyEditorMap.count(selectedComponent) > 0) {
        propertyEditorMap[selectedComponent]->onLeftUp(pos);
    }
}

void Editor::onAction(const Input::Action& action) {
    if(selectedComponent == ComponentType::None || selectedEntity == nullptr) {
        return;
    }
    if(propertyEditorMap.count(selectedComponent) > 0) {
        propertyEditorMap[selectedComponent]->onAction(action);
    }
}

void Editor::mainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("Level")) {
            if (ImGui::MenuItem("New")) {
                //stealFocusNextFrame = true;
            }
            if (ImGui::MenuItem("Load")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".json", ".", 1,
                                                        nullptr, ImGuiFileDialogFlags_Modal);
            }
            if (ImGui::MenuItem("Save")) {
                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save File", ".json", ".", 1,
                                                        nullptr, ImGuiFileDialogFlags_Modal);
            }
            if (ImGui::MenuItem("Properties")) {
                showProperties = true;
            }
            if (ImGui::MenuItem("Exit")) {
                //stealFocusNextFrame = true;
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Entity")) {
            if (ImGui::MenuItem("Create")) {
                stealFocusNextFrame = true;
                showCreateEntityModal = true;
            }
            if(ImGui::BeginMenu("Rename", selectedEntity != nullptr)) {
                //ImGui::SetNextItemWidth(150);
                ImGui::InputText("Name", &selectedEntity->name);
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", nullptr, false, selectedEntity != nullptr)) {
                SDL_Log("Delete entity");
                world->destroy(selectedEntity, true);
                clearSelection();
                metaData.clear();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Component")) {
            if (ImGui::BeginMenu("Create", selectedEntity != nullptr)) {
                assignComponentMenu();
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", nullptr, false, selectedEntity != nullptr && selectedComponent != ComponentType::None)) {
                SDL_Log("Delete component");
                removeComponent(selectedEntity, selectedComponent);
                clearSelection();
                metaData.clear();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Options")) {
            if(ImGui::BeginMenu("Grid")) {
                ImGui::SetNextItemWidth(150);
                ImGui::InputInt("Size", &currentGridSize);
                setGridSize((float) currentGridSize, (float) currentGridSize);
                ImGui::EndMenu();
            }
            ImGui::MenuItem("Show UI Demo", nullptr, &showDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::fileDialogs() {
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(700, 450)))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string curPath(getcwd(nullptr,0));
            filePathName = filePathName.substr(curPath.size()+1);
            //SDL_Log("filePathName: %s, filePath: %s, getCwd: %s", filePathName.c_str(), filePath.c_str(), curPath.c_str());
            level.transitionToLevel(filePathName, [this](){
                reset();
            });
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(700, 450)))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string curPath(getcwd(nullptr,0));
            filePathName = filePathName.substr(curPath.size()+1);
            //SDL_Log("filePathName: %s, filePath: %s, getCwd: %s", filePathName.c_str(), filePath.c_str(), curPath.c_str());
            level.save(filePathName);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void Editor::assignComponentMenu() {
    if(ImGui::MenuItem("Transform", nullptr, false, !selectedEntity->has<TransformComponent>())) {
        selectedEntity->assign<TransformComponent>(Vector2(0,0), 1.0f, 0, 0);
    }
    if(ImGui::MenuItem("Terrain", nullptr, false, !selectedEntity->has<TerrainComponent>())) {
        selectedEntity->assign<TerrainComponent>(std::vector<Vector2> {
            {300 + camera.scrollX,500 + camera.scrollY},{500 + camera.scrollX,500 + camera.scrollY},
            {500 + camera.scrollX,550 + camera.scrollY},{300 + camera.scrollX,550 + camera.scrollY}},true);
    }
    //ImGui::MenuItem("Terrain");
}

void Editor::createEntityModal() {
    static char nameBuf[128] = {0};

    ImGui::OpenPopup("Create Entity");
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create Entity", &showCreateEntityModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));
        ImGui::Text("Creates a new entity with the given name.\nUse Entity->Assign Component menu to assign components\n\n");
        ImGui::Spacing();
        if(stealFocusNextFrame) {
            ImGui::SetKeyboardFocusHere();
            stealFocusNextFrame = false;
        }

        ImGui::InputText("Name", nameBuf, IM_ARRAYSIZE(nameBuf));
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            std::string name(nameBuf);
            if(!name.empty()) {
                Entity* ent = world->create();
                ent->setName(name);
                showCreateEntityModal = false;
                selectedEntity = ent;
                selectedComponent = ComponentType::None;
                std::memset(nameBuf, 0, sizeof(nameBuf));
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SetItemDefaultFocus();
        //ImGui::SameLine();
        ImGui::SameLine(ImGui::GetWindowWidth()-130);
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showCreateEntityModal = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
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

        bool node_open = ImGui::TreeNode(entity->getName().c_str());
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
                if (ImGui::BeginPopupContextItem("componentContextMenu")) {
                    if (ImGui::Selectable("Delete")) {
                        SDL_Log("Clicked delete");
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
            if(metaData[entityId].componentTypes.empty()) {
                bool isSelected = false;
                if(selectedEntity == entity) {
                    isSelected = true;
                }
                if(ImGui::Selectable("empty", isSelected)) {
                    selectedEntity = entity;
                    selectedComponent = ComponentType::None;
                }
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    ImGui::Columns(1);
    //ImGui::Separator();
    ImGui::PopStyleVar();
    ImGui::End();
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

void Editor::properties() {
    ImGui::SetNextWindowSize(ImVec2(750,450), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Level Properties", &showProperties);
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("PropertiesTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Level"))
        {
            ImGui::Text("Skycolor, height width and stuff");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Background"))
        {
            backgroundEditor();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Spawnpoints"))
        {
            ImGui::Text("spawnpoint editor");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Editor::backgroundEditor() {
    auto& scroller = level.getScroller();
    static u32 selected = 0;
    ImGui::BeginGroup();
    ImGui::BeginChild("left pane", ImVec2(120, -ImGui::GetFrameHeightWithSpacing()), true);
        for (u32 i = 0; i < scroller.layers.size(); i++) {
            if (ImGui::Selectable(string_format("Layer %d", i).c_str(), selected == i))
                selected = i;

        }
    ImGui::EndChild();
    if (ImGui::Button("+", ImVec2(25, 0))) {
        scroller.addLayer(selected);
    }
    ImGui::SameLine();
    if (ImGui::Button("-", ImVec2(25, 0))) {
        scroller.deleteLayer(selected);
    }
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::BeginGroup();
    if(!scroller.layers.empty()) {
        ImGui::BeginChild("item view", ImVec2(0,
                                              -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        auto &layer = scroller.layers[selected];
        ImGui::InputText("Filename", &layer.filename);
        ImGui::SameLine();
        if (ImGui::Button("Browse")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseLayerImageKey", "Choose Image", ".png",
                                                    ".", 1,
                                                    nullptr, ImGuiFileDialogFlags_Modal);
        }
        ImGui::InputFloat("Width", &layer.width, 0, 0, "%.0f");
        ImGui::InputFloat("Height", &layer.height, 0, 0, "%.0f");
        ImGui::InputFloat("PositionY", &layer.posY, 0, 0, "%.0f");
        ImGui::InputFloat("SpeedFactor", &layer.speedFac, 0, 0, "%.02f");
        ImGui::Checkbox("Foreground", &layer.foreground);
        ImGui::Spacing();
        if (ImGui::Button("Move Up")) {
            if (scroller.moveLayerUp(selected)) {
                selected--;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Move Down")) {
            if (scroller.moveLayerDown(selected)) {
                selected++;
            }
        }
        if (!layer.built) {
            ImGui::Spacing();
            ImGui::Text("This layer needs to be rebuild.");
        }
        ImGui::EndChild();

        //ImGui::SameLine();
        if (ImGui::Button("Rebuild")) {
            scroller.rebuild();
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseLayerImageKey", ImGuiWindowFlags_NoCollapse, ImVec2(700, 450))) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string curPath(getcwd(nullptr,0));
                filePathName = filePathName.substr(curPath.size()+1);
                layer.filename = filePathName;
                //SDL_Log("filePathName: %s, filePath: %s, getCwd: %s", filePathName.c_str(), filePath.c_str(), curPath.c_str());
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }
    ImGui::EndGroup();
}

void Editor::clearSelection() {
    selectedEntity = nullptr;
    selectedComponent = ComponentType::None;
    resetEditors();
}

static void insertComponentIfNotExist(std::map<ComponentType, bool> &componentTypes, ComponentType type) {
    if(componentTypes.count(type) == 0) {
        componentTypes[type] = false;
    }
}

void Editor::removeComponent(Entity* ent, ComponentType type) {
    switch(type) {
        case ComponentType::None: break;
        case ComponentType::Transform: ent->remove<TransformComponent>(); break;
        case ComponentType::Terrain: ent->remove<TerrainComponent>(); break;
        case ComponentType::Ladder: ent->remove<LadderComponent>(); break;
        case ComponentType::Image: ent->remove<ImageComponent>(); break;
        case ComponentType::Platform: ent->remove<PlatformComponent>(); break;
        case ComponentType::Path: ent->remove<PathComponent>(); break;
        case ComponentType::Collision: ent->remove<CollisionComponent>(); break;
        case ComponentType::PointLight: ent->remove<PointLightComponent>(); break;
        case ComponentType::FlickerEffect: ent->remove<FlickerEffectComponent>(); break;
        case ComponentType::GlowEffect: ent->remove<GlowEffectComponent>(); break;
    }
}

void Editor::updateMetaData() {
    //metaData.clear();
    for (Entity *ent : world->all(false)) {

        if(metaData.count(ent->getEntityId()) == 0) {
            EntityMetaData entityMetaData;
            entityMetaData.entity = ent;
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

/*
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
 */