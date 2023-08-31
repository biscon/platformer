//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_COLLISIONINFOCOMPONENT_H
#define PLATFORMER_COLLISIONINFOCOMPONENT_H


#include <Vector2.h>

struct CollisionInfo {
    bool above = false;
    bool below = false;
    bool left = false;
    bool right = false;
    bool climbingSlope = false;
    bool descendingSlope = false;
    bool slidingDownMaxSlope = false;
    bool ascending = false;
    bool onLadder = false;
    bool onTopLadder = false;
    bool onDoor = false;
    float slopeAngle = 0;
    Vector2 velocity;
    float distanceGround = 0;
    float distanceLadderCenter = 0;
    Vector2 slopeNormal;

    void reset() {
        above = false;
        below = false;
        left = false;
        right = false;
        climbingSlope = false;
        descendingSlope = false;
        slidingDownMaxSlope = false;
        ascending = false;
        onLadder = false;
        onDoor = false;
        onTopLadder = false;
        slopeAngle = 0;
        distanceGround = 0;
        distanceLadderCenter = 0;
        slopeNormal.zero();
    }
};

struct CollisionInfoComponent {
    CollisionInfo info[2];
    CollisionInfo& cur() { return info[0]; }
    CollisionInfo& prev() { return info[1]; }
};


#endif //PLATFORMER_COLLISIONINFOCOMPONENT_H
