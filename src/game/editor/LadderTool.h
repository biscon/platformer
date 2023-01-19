//
// Created by bison on 24-12-22.
//

#ifndef PLATFORMER_LADDERTOOL_H
#define PLATFORMER_LADDERTOOL_H

#include "RectangleTool.h"

using namespace Renderer;
using namespace Input;

class LadderTool : public RectangleTool {
public:
    LadderTool(IInputDevice &inputDevice, RenderCmdBuffer &buffer, Font &font, Camera &camera,
            World *world) : RectangleTool(inputDevice, buffer, font, camera, world, true) {}

    void onUpdate() override;
    void setSelected(Entity *ent) override;
    Entity *getNewSelected() override;
    void reset() override;

private:
    Entity* newSelected = nullptr;
    void onRectComplete() override;
    void onResizeComplete() override;
    void onResize() override;
    void buildRect();
    void renderTextRight(const std::string& text, i32 line);
    void renderInfo();
};


#endif //PLATFORMER_LADDERTOOL_H
