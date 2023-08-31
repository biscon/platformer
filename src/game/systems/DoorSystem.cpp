//
// Created by bison on 22-11-22.
//

#include <SDL_log.h>
#include "DoorSystem.h"
#include "../Utils.h"
#include "../AABB.h"
#include "../components/ActorComponent.h"

void DoorSystem::tick(World *world, float deltaTime) {
    for (Entity* ent : world->all(false)) {
        auto input = ent->get<InputComponent>();
        auto colInfo = ent->get<CollisionInfoComponent>();
        auto actor = ent->get<ActorComponent>();
        if(!input.isValid() || !actor.isValid() || !colInfo.isValid()) {
            continue;
        }
        Entity* hitDoorEnt = senseDoor(ent);
        if((input->cur().up && !input->prev().up && colInfo->cur().onDoor) || (input->cur().down && !input->prev().down && colInfo->cur().onDoor)) {
            auto hitDoor = hitDoorEnt->get<DoorComponent>();
            auto sprite = hitDoorEnt->get<SpriteComponent>();
            if(hitDoor.isValid()) {
                SDL_Log("on door enter");
                if(sprite.isValid()) {
                    sprite->resume();
                }
                levelChangeCallback(hitDoor->levelFileName, hitDoor->spawnId);
            }
        }
    }
}

Entity* DoorSystem::senseDoor(Entity *ent) {
    auto colInfo = ent->get<CollisionInfoComponent>();
    if(!colInfo.isValid()) {
        return nullptr;
    }
    FloatRect bounds;
    bounds.zero();
    getBounds(ent, bounds);

    AABB entAABB(bounds);
    AABB midSensor;
    midSensor.center.x = entAABB.center.x;
    midSensor.center.y = entAABB.center.y;
    midSensor.half.x = 1;
    midSensor.half.y = entAABB.half.y - 2.0f;

    for (Entity* doorEnt : ent->getWorld()->each<DoorComponent>()) {
        auto door = doorEnt->get<DoorComponent>();

        AABB doorAABB(door->rect);
        Hit hit;
        if(midSensor.intersectAABB(doorAABB, hit)) {
            colInfo->cur().onDoor = true;
            return doorEnt;
        }
    }
    return nullptr;
}
