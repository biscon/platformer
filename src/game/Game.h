//
// Created by bison on 4/4/19.
//

#ifndef GAME_H
#define GAME_H

#include <memory>
#include <ECS.h>
#include <RenderBuffers.h>
#include "../renderer/RenderCmdBuffer.h"
#include "../renderer/Font.h"
#include "../input/Input.h"
#include "systems/RenderDebugSystem.h"
#include "Level.h"
#include "editor/Editor.h"

using namespace ECS;

class Game {
public:
    Game(Input::IInputDevice& inputDevice, Renderer::IRenderDevice& renderDevice, RenderBuffers renderBuffers);

    virtual ~Game();

    bool update(double frameDelta);

private:
    Input::IInputDevice& inputDevice;
    Renderer::IRenderDevice& renderDevice;
    RenderBuffers renderBuffers;
    std::shared_ptr<Input::InputContext> inputContext;
    std::unique_ptr<Renderer::Font> font;
    std::unique_ptr<Renderer::Font> debugFont;
    std::shared_ptr<Level> level;
    std::unique_ptr<Editor> editor;
    bool showFps = false;
    bool editMode = false;

    void processEditModeInput(float deltaTime);

    void setupInputMapping();
};

#endif
