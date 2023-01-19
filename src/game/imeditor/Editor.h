//
// Created by bison on 18-01-23.
//

#ifndef PLATFORMER_EDITOR_H
#define PLATFORMER_EDITOR_H

#include <memory>
#include <ECS.h>
#include <RenderBuffers.h>
#include "../../renderer/RenderCmdBuffer.h"
#include "../Level.h"

using namespace ECS;
using namespace Renderer;

class Editor {
public:
    explicit Editor(std::shared_ptr<Level> level, IInputDevice &inputDevice, RenderBuffers renderBuffers, Font &font);

    void update(float deltaTime);

private:
    std::shared_ptr<Level> level;
    IInputDevice &inputDevice;
    RenderBuffers renderBuffers;
    Font &font;
    Vector2 camTarget;

};


#endif //PLATFORMER_EDITOR_H
