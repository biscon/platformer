//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_PHYSICCOMPONENT_H
#define PLATFORMER_PHYSICCOMPONENT_H

#include <cmath>
#include <Vector2.h>

struct PhysicComponent {
    PhysicComponent() {
        gravity = (2 * maxJumpHeight) / powf(timeToJumpApex, 2);
        maxJumpVelocity = fabsf(gravity * timeToJumpApex);
        minJumpVelocity = sqrtf(2 * fabsf(gravity) * minJumpHeight);

    }

    float gravity;
    float moveSpeed = 600;
    float ladderVertSpeed = 300;
    float ladderHorizSpeed = 200;
    float maxJumpHeight = 300;
    float minJumpHeight = maxJumpHeight / 4.0f;
    float timeToJumpApex = .4f;
    float maxJumpVelocity;
    float minJumpVelocity;
    float accTimeAirborne = .2f;
    float accTimeGrounded = .05f;
    float accTimeLadderCenter = .01f;
    Vector2 velocity;
    float velocityXSmoothing = 0;
    float velocityYSmoothing = 0;
};

#endif //PLATFORMER_PHYSICCOMPONENT_H
