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
#include "../Level.h"

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
    std::map<ComponentType, bool> componentTypes;
};

class Editor {
public:
    explicit Editor(IRenderDevice& renderDevice, IInputDevice &inputDevice, World* world, Camera& camera, RenderBuffers renderBuffers, Font &font, Level &level);

    void update(float deltaTime);
    void reset();

private:
    IRenderDevice& renderDevice;
    IInputDevice &inputDevice;
    World *world;
    Camera& camera;
    RenderBuffers buffers;
    Font &font;
    std::shared_ptr<Input::InputContext> inputContext;
    Level& level;

    i32 selectedTool = 0;
    Entity* selected = nullptr;
    i32 currentGridSize = 10;

    std::unordered_map<size_t, EntityMetaData> metaData;
    Entity* selectedEntity = nullptr;
    ComponentType selectedComponent = ComponentType::None;
    std::unordered_map<ComponentType, std::unique_ptr<IComponentPropertyEditor>> propertyEditorMap;
    bool showCreateEntityModal = false;
    bool stealFocusNextFrame = false;
    bool showDemo = false;
    bool showProperties = false;

    void resetEditors();
    void setSelected(Entity* ent);
    void duplicate(Entity* ent);
    void onDelete();
    void onLeftDown(float x, float y);
    void onLeftUp(float x, float y);

    void showEntityComponentSelector();
    void updateMetaData();
    void showComponentProperties();
    void mainMenu();
    void createEntityModal();
    void assignComponentMenu();
    void fileDialogs();

    void onAction(const Action &action);
    void clearSelection();
    void removeComponent(Entity *ent, ComponentType type);
    void properties();

    void levelProperties();
    void backgroundProperties();
};

std::string getComponentName(ComponentType type);

#endif //PLATFORMER_EDITOR_H
