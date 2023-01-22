//
// Created by bison on 22-01-23.
//

#ifndef PLATFORMER_TERRAINPROPERTYEDITOR_H
#define PLATFORMER_TERRAINPROPERTYEDITOR_H


#include "IComponentPropertyEditor.h"
#include "../Camera.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../../input/Input.h"
#include "../components/TerrainComponent.h"
#include "RenderBuffers.h"

using namespace Renderer;
using namespace Input;

class TerrainPropertyEditor: public IComponentPropertyEditor {
public:
    TerrainPropertyEditor(IInputDevice &inputDevice, RenderBuffers buffers, Font &font, Camera &camera,
            World *world) : inputDevice(inputDevice), buffers(buffers), font(font), camera(camera),
    world(world) {}
    void show() override;
    void setSelected(Entity *ent) override;

private:
    IInputDevice &inputDevice;
    RenderBuffers buffers;
    Font &font;
    Camera& camera;
    World *world;

    Entity* selected = nullptr;
    bool editing = false;

    void propertyWindow();

    void editWindow();

    void deletePrompt();

    void renderSelection();
};


#endif //PLATFORMER_TERRAINPROPERTYEDITOR_H
