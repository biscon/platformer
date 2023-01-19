//
// Created by bison on 06-01-23.
//

#ifndef PLATFORMER_LIGHTTOOL_H
#define PLATFORMER_LIGHTTOOL_H


#include <ECS.h>
#include <RenderBuffers.h>
#include "ITool.h"
#include "../Camera.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../../input/Input.h"

using namespace Renderer;
using namespace Input;

class LightTool : public ITool {
public:
    LightTool(IInputDevice &inputDevice, RenderBuffers buffers, Font &font, Camera &camera,
                  World *world) : inputDevice(inputDevice), buffers(buffers), font(font), camera(camera),
                                                       world(world) {}

    void onUpdate() override;

    bool onKeyboard(MappedId key) override;

    bool onLeftDown(Vector2 pos) override;

    bool onLeftUp(Vector2 pos) override;

    void reset() override;

    void setSelected(Entity *ent) override;

    Entity *getNewSelected() override;

protected:
    IInputDevice &inputDevice;
    RenderBuffers buffers;
    Font &font;
    Camera& camera;
    World *world;

    Vector2 orgVertex;

private:
    Entity* selected = nullptr;
    Entity* newSelected = nullptr;
    const float vertexHandleSize = 8.0f;
    bool canEdit = false;
    i32 selectedHandle = -1;
    std::vector<Vector2> handles;
    bool moving = false;
    Vector2 moveStart;

    void render(Entity* ent);
    void renderHandle(Vector2 vertex, const Color& color);
    void renderTextRight(const std::string& text, i32 line);
    void renderInfo();
    void setHandles();
    void moveHandle();
    void updateLight();
    i32 selectHandle(const Vector2& pos);
    void startMovingHandle(Vector2& pos, Vector2 vertex);

};

#endif //PLATFORMER_LIGHTTOOL_H
