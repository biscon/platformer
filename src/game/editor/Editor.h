//
// Created by bison on 20-12-22.
//

#ifndef PLATFORMER_EDITOR_H
#define PLATFORMER_EDITOR_H

#include <memory>
#include <ECS.h>
#include <RenderBuffers.h>
#include <unordered_set>
#include <map>
#include "../../renderer/RenderCmdBuffer.h"
#include "../../renderer/Gui.h"
#include "../Camera.h"
#include "ITool.h"

using namespace ECS;
using namespace Renderer;

struct ToolbarItem {
    explicit ToolbarItem(std::unique_ptr<ITool> tool) : tool(std::move(tool)) {}

    std::unique_ptr<ITool> tool;
    std::string name = "";
    ToggleButton button;
    MappedId inputId = 0;
};

enum class ComponentType
{
    None,
    Transform,
    Terrain,
    Ladder,
    Image,
    Platform,
    Path,
    Collision,
    PointLight,
    FlickerEffect,
    GlowEffect,
};

struct EntityMetaData {
    Entity *entity;
    std::string name;
    std::map<ComponentType, bool> componentTypes;
    //std::vector<ComponentType> componentTypes;
};

class Editor {
public:
    Vector2 camTarget;

    explicit Editor(std::unique_ptr<Gui> &gui, IInputDevice &inputDevice, World* world, Camera& camera, RenderBuffers renderBuffers, Font &font);

    void update(float deltaTime);
    void reset();

private:
    std::unique_ptr<Gui> ui;
    IInputDevice &inputDevice;
    World *world;
    Camera& camera;
    RenderBuffers buffers;
    Font &font;
    std::shared_ptr<Input::InputContext> inputContext;

    std::vector<std::unique_ptr<ToolbarItem>> toolbar;
    i32 selectedTool = 0;
    Entity* selected = nullptr;
    i32 currentGridSize = 2;

    std::unordered_map<size_t, EntityMetaData> metaData;
    Entity* selectedEntity = nullptr;
    ComponentType selectedComponent = ComponentType::None;


    void selectToolByInputId(MappedId inputId);
    void resetTools();
    void setSelected(Entity* ent);
    void duplicate(Entity* ent);
    void onDelete();
    void onLeftDown(float x, float y);
    void onLeftUp(float x, float y);
    void cycleGridSize();

    void showEntityComponentSelector();

    void updateMetaData();

    void showDeleteComponentPrompt();

    void showComponentProperties();
};

std::string getComponentName(ComponentType type);

#endif //PLATFORMER_EDITOR_H
