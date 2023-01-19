//
// Created by bison on 22-11-22.
//

#ifndef GAME_INPUTSYSTEM_H
#define GAME_INPUTSYSTEM_H
#include <ECS.h>
#include "../../input/Input.h"
#include <memory>

using namespace ECS;

class InputSystem : public EntitySystem {
public:
    explicit InputSystem(Input::IInputDevice& device);
    ~InputSystem() override = default;
    void tick(World* world, float deltaTime) override;

private:
    std::shared_ptr<Input::InputContext> inputContext;
};


#endif //GAME_SPRITESYSTEM_H
