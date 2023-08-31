//
// Created by bison on 22-11-22.
//

#ifndef GAME_DOORSYSTEM_H
#define GAME_DOORSYSTEM_H
#include <ECS.h>

#include <utility>
#include "../components/SpriteComponent.h"
#include "../components/DoorComponent.h"
#include "../components/InputComponent.h"
#include "../components/CollisionInfoComponent.h"

using namespace ECS;
using namespace Renderer;

class DoorSystem : public EntitySystem {
public:
    explicit DoorSystem(std::function<void(std::string filename, std::string spawnId)> levelChangeCallback) : levelChangeCallback(std::move(levelChangeCallback)) {}
    ~DoorSystem() override = default;
    void tick(World *world, float deltaTime) override;

private:
    std::function<void(std::string filename, std::string spawnId)> levelChangeCallback;
    Entity* senseDoor(Entity *ent);
};


#endif //GAME_DOORSYSTEM_H
