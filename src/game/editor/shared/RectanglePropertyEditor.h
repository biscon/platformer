//
// Created by bison on 30-01-23.
//

#ifndef PLATFORMER_RECTANGLEPROPERTYEDITOR_H
#define PLATFORMER_RECTANGLEPROPERTYEDITOR_H


#include "../IComponentPropertyEditor.h"
#include "../../../renderer/RenderCmdBuffer.h"
#include "../../Camera.h"


using namespace Renderer;
using namespace Input;

class RectanglePropertyEditor: public IComponentPropertyEditor {
public:
    RectanglePropertyEditor(IInputDevice &inputDevice, RenderCmdBuffer &buffer, Font &font, Camera &camera,
            World *world) : inputDevice(inputDevice), buffer(buffer), font(font), camera(camera),
    world(world) {}


    void show() override;
    void setSelected(Entity *ent) override;
    bool onLeftDown(Vector2 pos) override;
    bool onLeftUp(Vector2 pos) override;
    bool onAction(const Input::Action &action) override;

protected:
    IInputDevice &inputDevice;
    RenderCmdBuffer& buffer;
    Font &font;
    Camera& camera;
    World *world;

    std::unique_ptr<FloatRect> rect;
    Entity* selected = nullptr;

    bool resizing = false;
    Vector2 resizeStart;
    Vector2 orgVertex;

    bool moving = false;
    Vector2 moveStart;
    Vector2 orgPos;

private:
    const float vertexHandleSize = 8.0f;

    void flipIfInverted();
    void startMoving(const Vector2& pos);
    void startResizing(Vector2& pos);
    void move();
    void resize();
    void renderRect();
    void renderHandle(Vector2& vertex, const Color& color);

    virtual void onResizeComplete() = 0;
    virtual void onResize() = 0;
    virtual void onMove(const Vector2& newPos) = 0;
    virtual void buildRect() = 0;
};


#endif //PLATFORMER_RECTANGLEPROPERTYEDITOR_H
