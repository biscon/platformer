//
// Created by bison on 26-12-22.
//

#ifndef PLATFORMER_IMAGETOOL_H
#define PLATFORMER_IMAGETOOL_H

#include "RectangleTool.h"

using namespace Renderer;
using namespace Input;

class ImageTool : public RectangleTool {
public:
    ImageTool(IInputDevice &inputDevice, RenderCmdBuffer &buffer, Font &font, Camera &camera,
            World *world) : RectangleTool(inputDevice, buffer, font, camera, world, false) {}

    void onUpdate() override;
    void setSelected(Entity *ent) override;
    Entity *getNewSelected() override;
    void reset() override;
    bool onKeyboard(MappedId key) override;

private:
    void onRectComplete() override;
    void onResizeComplete() override;
    void onResize() override;
    void buildRect();
    void renderTextRight(const std::string& text, i32 line);
    void renderInfo();
};


#endif //PLATFORMER_IMAGETOOL_H
