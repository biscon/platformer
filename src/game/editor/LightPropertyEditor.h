//
// Created by bison on 30-01-23.
//

#ifndef PLATFORMER_LIGHTPROPERTYEDITOR_H
#define PLATFORMER_LIGHTPROPERTYEDITOR_H

#include "IComponentPropertyEditor.h"
#include "../Camera.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../../input/Input.h"
#include "RenderBuffers.h"

using namespace Renderer;
using namespace Input;

class LightPropertyEditor: public IComponentPropertyEditor {
public:
    LightPropertyEditor(IInputDevice &inputDevice, RenderBuffers buffers, Font &font, Camera &camera,
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
    Vector2 orgVertex;

    const float vertexHandleSize = 8.0f;
    i32 selectedHandle = -1;
    std::vector<Vector2> handles;
    bool movingHandle = false;
    Vector2 moveStart;
    Vector2 orgPos;
    bool moving = false;


    void render();
    void renderHandle(Vector2 vertex, const Color& color);
    void setHandles();
    void moveHandle();
    void updateLight();
    i32 selectHandle(const Vector2& pos);
    void startMovingHandle(Vector2& pos, Vector2 vertex);
    void move();
};


#endif //PLATFORMER_LIGHTPROPERTYEDITOR_H
