//
// Created by bison on 05-02-23.
//

#ifndef PLATFORMER_SPRITEPROPERTYEDITOR_H
#define PLATFORMER_SPRITEPROPERTYEDITOR_H

#include <RenderBuffers.h>
#include "../../renderer/RenderCmdBuffer.h"
#include "IComponentPropertyEditor.h"
#include "../../input/Input.h"
#include "../Camera.h"
#include "shared/RectanglePropertyEditor.h"

using namespace Input;
using namespace Renderer;

class SpritePropertyEditor: public RectanglePropertyEditor {
public:
    SpritePropertyEditor(IInputDevice &inputDevice, RenderBuffers buffers, Font &font, Camera &camera,
            World *world) : RectanglePropertyEditor(inputDevice, buffers.unlit, font, camera, world), buffers(buffers) {}

    void show() override;

private:
    RenderBuffers buffers;
    bool showAnimations = false;

    void onResizeComplete() override;
    void onResize() override;
    void onMove(const Vector2 &newPos, const Vector2& delta) override;
    void buildRect() override;
    void animationEditor();
};


#endif //PLATFORMER_SPRITEPROPERTYEDITOR_H
