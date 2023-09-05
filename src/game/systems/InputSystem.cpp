//
// Created by bison on 22-11-22.
//

#include <SDL_log.h>
#include "InputSystem.h"
#include "../components/InputComponent.h"

InputSystem::InputSystem(Input::IInputDevice &device) {
    inputContext = std::make_shared<Input::InputContext>();
    device.registerContext(inputContext);
    inputContext->registerState(INPUT_STATE_FORWARD);
    inputContext->registerState(INPUT_STATE_BACK);
    inputContext->registerState(INPUT_STATE_LEFT);
    inputContext->registerState(INPUT_STATE_RIGHT);
    inputContext->registerState(INPUT_STATE_JUMP);
    //inputContext->registerAction(INPUT_ACTION_SPACE);
}

void InputSystem::tick(World *world, float deltaTime) {
    /*
    Input::Action action = {};
    auto jump = false;
    while(inputContext->pollAction(action)) {
        if(action.id == INPUT_ACTION_SPACE) {
            jump = true;
        }
    }
    */
    for (Entity* ent : world->all(false)) {
        auto input = ent->get<InputComponent>();
        if(!input.isValid()) {
            continue;
        }
        input->inputs[1] = input->inputs[0];
        input->inputs[0].up = inputContext->queryState(INPUT_STATE_FORWARD);
        input->inputs[0].down = inputContext->queryState(INPUT_STATE_BACK);
        input->inputs[0].left = inputContext->queryState(INPUT_STATE_LEFT);
        input->inputs[0].right = inputContext->queryState(INPUT_STATE_RIGHT);
        input->inputs[0].jump = inputContext->queryState(INPUT_STATE_JUMP);
    }
}

