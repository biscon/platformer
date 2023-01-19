//
// Created by bison on 23-12-22.
//

#ifndef PLATFORMER_RECTANGLETOOL_H
#define PLATFORMER_RECTANGLETOOL_H


#include <ECS.h>
#include "ITool.h"
#include "../Camera.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../../input/Input.h"
#include "../components/TerrainComponent.h"

using namespace Renderer;
using namespace Input;

class RectangleTool : public ITool {
public:
    RectangleTool(IInputDevice &inputDevice, RenderCmdBuffer &buffer, Font &font, Camera &camera,
            World *world, bool drawingEnabled) : inputDevice(inputDevice), buffer(buffer), font(font), camera(camera),
    world(world), drawingEnabled(drawingEnabled) {}

    void onUpdate() override;
    bool onKeyboard(MappedId key) override;
    bool onLeftDown(Vector2 pos) override;
    bool onLeftUp(Vector2 pos) override;
    void reset() override;

protected:
    IInputDevice &inputDevice;
    RenderCmdBuffer& buffer;
    Font &font;
    Camera& camera;
    World *world;
    bool drawingEnabled = true;

    std::unique_ptr<FloatRect> rect;
    Entity* selected = nullptr;

    bool resizing = false;
    Vector2 resizeStart;
    Vector2 orgVertex;

private:
    const float vertexHandleSize = 8.0f;
    bool drawing = false;

    void flipIfInverted();
    void startResizing(Vector2& pos);
    void resize();
    void drawingRect();
    void renderRect();
    void renderHandle(Vector2& vertex, const Color& color);

    virtual void onRectComplete() = 0;
    virtual void onResizeComplete() = 0;
    virtual void onResize() = 0;
};


#endif //PLATFORMER_RECTANGLETOOL_H
