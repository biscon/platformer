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

    bool onLeftDown(Vector2 pos) override;
    bool onLeftUp(Vector2 pos) override;

    bool onAction(const Action &action) override;

private:
    IInputDevice &inputDevice;
    RenderBuffers buffers;
    Font &font;
    Camera& camera;
    World *world;

    Entity* selected = nullptr;
    bool editing = false;

    i32 selectedVertex = -1;
    bool movingVertex = false;
    Vector2 moveStartVertex;
    Vector2 orgVertex;
    const Color LAST_SEGMENT_COLOR = {0.0f, 1.0f, 0.0f, 0.5f};
    const float vertexHandleSize = 8.0f;

    void propertyWindow();
    void editWindow();
    void deletePrompt();
    void renderSelection();
};


#endif //PLATFORMER_TERRAINPROPERTYEDITOR_H
