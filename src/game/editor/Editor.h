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
#include "../Camera.h"
#include "ITool.h"
#include "IComponentPropertyEditor.h"

using namespace ECS;
using namespace Renderer;

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

    explicit Editor(IInputDevice &inputDevice, World* world, Camera& camera, RenderBuffers renderBuffers, Font &font);

    void update(float deltaTime);
    void reset();

private:
    IInputDevice &inputDevice;
    World *world;
    Camera& camera;
    RenderBuffers buffers;
    Font &font;
    std::shared_ptr<Input::InputContext> inputContext;

    i32 selectedTool = 0;
    Entity* selected = nullptr;
    i32 currentGridSize = 2;

    std::unordered_map<size_t, EntityMetaData> metaData;
    Entity* selectedEntity = nullptr;
    ComponentType selectedComponent = ComponentType::None;

    std::unordered_map<ComponentType, std::unique_ptr<IComponentPropertyEditor>> propertyEditorMap;

    void resetEditors();
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
