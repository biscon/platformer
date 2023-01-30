//
// Created by bison on 29-01-23.
//

#ifndef PLATFORMER_IMAGEPROPERTYEDITOR_H
#define PLATFORMER_IMAGEPROPERTYEDITOR_H

#include <RenderBuffers.h>
#include "../../renderer/RenderCmdBuffer.h"
#include "IComponentPropertyEditor.h"
#include "../../input/Input.h"
#include "../Camera.h"
#include "shared/RectanglePropertyEditor.h"

using namespace Input;
using namespace Renderer;

class ImagePropertyEditor: public RectanglePropertyEditor {
public:
    ImagePropertyEditor(IInputDevice &inputDevice, RenderBuffers buffers, Font &font, Camera &camera,
            World *world) : RectanglePropertyEditor(inputDevice, buffers.unlit, font, camera, world), buffers(buffers) {}

    void show() override;

private:
    RenderBuffers buffers;
    void onResizeComplete() override;
    void onResize() override;
    void onMove(const Vector2 &newPos) override;
    void buildRect() override;
};


#endif //PLATFORMER_IMAGEPROPERTYEDITOR_H
