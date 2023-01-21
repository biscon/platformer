//
// Created by bison on 20-12-22.
//

#ifndef PLATFORMER_EDITOR_H
#define PLATFORMER_EDITOR_H

#include <memory>
#include <ECS.h>
#include <RenderBuffers.h>
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


class ComponentType
{
public:
    enum Value
    {
        Transform,
        Terrain,
    };

    ComponentType() = default;
    constexpr explicit ComponentType(Value aComponentType) : value(aComponentType) { }
    // Allow switch and comparisons.
    constexpr explicit operator Value() const { return value; }
    constexpr bool operator==(ComponentType a) const { return value == a.value; }
    constexpr bool operator!=(ComponentType a) const { return value != a.value; }
    
    //constexpr bool IsYellow() const { return value == Transform; }

    std::string name() {
        switch(value) {
            case Transform: return "Transform";
            case Terrain: return "Terrain";

        }
        return "Unknown";
    }

private:
    Value value;
};

struct EntityMetaData {
    Entity *entity;
    std::string name;
    std::vector<ComponentType> componentTypes;
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


    void selectToolByInputId(MappedId inputId);
    void resetTools();
    void setSelected(Entity* ent);
    void duplicate(Entity* ent);
    void onDelete();
    void onLeftDown(float x, float y);
    void onLeftUp(float x, float y);
    void cycleGridSize();

    void renderToolButtons();
};


#endif //PLATFORMER_EDITOR_H
