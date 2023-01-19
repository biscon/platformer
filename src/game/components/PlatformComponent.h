//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_PLATFORMCOMPONENT_H
#define PLATFORMER_PLATFORMCOMPONENT_H

#include <vector>
#include <ECS.h>
#include "../../renderer/Types.h"
#include "../JsonUtil.h"

struct Passenger {
    Passenger(ECS::Entity *entity, const Vector2 &velocity, bool standingOnPlatform,
              bool moveBeforePlatform) : entity(entity), velocity(velocity),
                                         standingOnPlatform(standingOnPlatform),
                                         moveBeforePlatform(moveBeforePlatform) {}

    ECS::Entity *entity = nullptr;
    Vector2 velocity;
    bool standingOnPlatform = false;
    bool moveBeforePlatform = false;
};

struct PlatformComponent {
    PlatformComponent(const Vector2& s, float speed) : size(s), speed(speed) {}
    PlatformComponent(const PlatformComponent& old) {
        size = old.size;
        speed = old.speed;
    }

    explicit PlatformComponent(const json& e);

    Vector2 size;
    float speed = 0;
    float waitTime = 1.0f;
    float waitTimer = 0;
    float pctBetweenWaypoints = 0;
    float easeAmount = 1.0f;
    Vector2 velocity;
    std::vector<Passenger> passengers;
    Vector2 moveRequest;

    void save(json& e);
};


#endif //PLATFORMER_PLATFORMCOMPONENT_H
