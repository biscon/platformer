//
// Created by bison on 21-12-22.
//

#ifndef PLATFORMER_PATHTOOL_H
#define PLATFORMER_PATHTOOL_H

#include <ECS.h>
#include "ITool.h"
#include "../Camera.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../../input/Input.h"
#include "../components/TerrainComponent.h"
#include "../components/PathComponent.h"

using namespace Renderer;
using namespace Input;


class PathTool : public ITool {
public:
    PathTool(IInputDevice &inputDevice, RenderCmdBuffer &buffer, Font &font, Camera &camera,
            World *world) : inputDevice(inputDevice), buffer(buffer), font(font), camera(camera),
    world(world) {}

    void onUpdate() override;

    bool onKeyboard(MappedId key) override;
    bool onHide();
    bool onDelete();
    bool onInsert();
    bool onEnter();

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

    PathComponent* selectedPath = nullptr;
    i32 selectedVertex = -1;
    bool moving = false;
    bool makingNew = false;
    Vector2 moveStart;
    Vector2 orgVertex;
    Entity* selected = nullptr;
    const Color LAST_SEGMENT_COLOR = {0.0f, 1.0f, 0.0f, 0.5f};
    const float vertexHandleSize = 8.0f;

    void startMovingVertex(Vector2& pos, Vector2 vertex);
    void render(PathComponent* path);
    void renderVertex(Vector2& vertex, const Color& color);
    void moveVertex();
    i32 selectVertex(Vector2& pos);

    void renderInfo();
    void renderTextRight(const std::string& text, i32 line);
};

#endif
