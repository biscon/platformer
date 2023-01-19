//
// Created by bison on 21-12-22.
//

#ifndef PLATFORMER_MOVETOOL_H
#define PLATFORMER_MOVETOOL_H

#include <ECS.h>
#include "ITool.h"
#include "../Camera.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../../input/Input.h"

using namespace Renderer;
using namespace Input;

class MoveTool : public ITool {
public:
    MoveTool(IInputDevice &inputDevice, RenderCmdBuffer &buffer, Font &font, Camera &camera,
             World *world) : inputDevice(inputDevice), buffer(buffer), font(font), camera(camera),
                             world(world) {}

    void onUpdate() override;

    bool onKeyboard(MappedId key) override;

    bool onLeftDown(Vector2 pos) override;
    bool onLeftUp(Vector2 pos) override;
    void reset() override;
    void setSelected(Entity *ent) override;
    Entity *getNewSelected() override;

private:
    IInputDevice &inputDevice;
    RenderCmdBuffer& buffer;
    Font &font;
    Camera& camera;
    World *world;

    bool moving = false;
    Vector2 moveStart;
    std::vector<Vector2> orgPoly;
    FloatRect orgRect;
    Vector2 orgPos;
    Entity* selected = nullptr;

    void move();
    void renderOutline();
    void renderInfo();
    void renderTextRight(const std::string& text, i32 line);
};


#endif //PLATFORMER_MOVETOOL_H
