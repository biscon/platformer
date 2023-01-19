//
// Created by bison on 01-01-23.
//

#include <unordered_set>
#include <SDL_log.h>
#include <Vector2.h>
#include "PlatformSystem.h"
#include "../components/TerrainComponent.h"
#include "../Utils.h"
#include "../PolyUtil.h"

void PlatformSystem::tick(World *world, float deltaTime) {
    for (Entity *ent : world->all(false)) {
        auto transform = ent->get<TransformComponent>();
        auto movingPlatform = ent->get<PlatformComponent>();
        auto path = ent->get<PathComponent>();

        if(!transform.isValid() || !movingPlatform.isValid()) {
            continue;
        }

        rebuildMovingPlatform(ent);

        Vector2 velocity;

        if(path.isValid()) {
            if (movingPlatform->pctBetweenWaypoints >= 1.0f) {
                //SDL_Log("pct: %f", movingPlatform->pctBetweenWaypoints);
                if (movingPlatform->waitTimer == 0) {
                    movingPlatform->waitTimer = movingPlatform->waitTime;
                }
                if (movingPlatform->waitTimer > 0) {
                    movingPlatform->waitTimer -= deltaTime;
                } else {
                    movingPlatform->waitTimer = 0;
                    movingPlatform->pctBetweenWaypoints = 0;
                    path->next();
                    velocity = calcPlatformMovement(transform.get(), movingPlatform.get(),
                                                    path.get(), deltaTime);
                }
                //SDL_Log("distance = %.2f, wp done, nextWP = %d, dir = %d", d, path->nextWaypoint, path->direction);
            } else {
                velocity = calcPlatformMovement(transform.get(), movingPlatform.get(), path.get(),
                                                deltaTime);
            }
        } else {
            if(movingPlatform->moveRequest.x != 0 && movingPlatform->moveRequest.y != 0) {
                velocity = movingPlatform->moveRequest;
            }
            movingPlatform->moveRequest.zero();
        }
        //velocity.round();
        calcPassengerMovement(ent, velocity);
        movingPlatform->velocity = velocity;
    }
}

Vector2 PlatformSystem::calcPlatformMovement(TransformComponent& transform, PlatformComponent& platform, PathComponent& path, float deltaTime) {
    Vector2 fromWp = path.fromWaypoint();
    Vector2 toWp = path.toWaypoint();
    float d = distance(fromWp, toWp);
    platform.pctBetweenWaypoints += deltaTime * platform.speed/d;
    platform.pctBetweenWaypoints = std::clamp(platform.pctBetweenWaypoints, 0.0f, 1.0f);
    float easedPctBetweenWaypoints = ease(platform.pctBetweenWaypoints, platform.easeAmount);

    Vector2 velocity(glm_lerp(fromWp.x, toWp.x, easedPctBetweenWaypoints), glm_lerp(fromWp.y, toWp.y, easedPctBetweenWaypoints));
    velocity -= transform.pos;
    return velocity;
}

void PlatformSystem::calcPassengerMovement(Entity *ent, Vector2 velocity) {
    auto transform = ent->get<TransformComponent>();
    auto movingPlatform = ent->get<PlatformComponent>();
    auto raycast = ent->get<CollisionComponent>();

    if(!transform.isValid() || !raycast.isValid()) {
        return;
    }

    movingPlatform->passengers.clear();

    std::unordered_set<Entity*> movedPassengers;
    float directionX = glm_signf(velocity.x);
    float directionY = glm_signf(velocity.y);

    FloatRect bounds(transform->pos.x, transform->pos.y,
            transform->pos.x + movingPlatform->size.x, transform->pos.y + movingPlatform->size.y);


    // vertically moving platform
    calculateRaySpacing(raycast.get(), bounds);
    updateRaycastOrigins(raycast.get(), bounds);

    if(velocity.y != 0) {
        float rayLength = (directionY == -1) ? velocity.y - raycast->skinWidth : velocity.y + raycast->skinWidth;
        for(i32 i = 0; i < raycast->verticalRayCount; i++) {
            Vector2 rayOrigin = (directionY == 1) ? raycast->raycastOrigins.bottomLeft : raycast->raycastOrigins.topLeft;
            rayOrigin.x += raycast->verticalRaySpacing * i;
            Vector2 rayEnd(rayOrigin.x, rayOrigin.y + rayLength);

            RaycastHit hit;
            if(performRaycastActor(ent->getWorld(), rayOrigin, rayEnd, hit)) {
                if(movedPassengers.count(ent) == 0) {
                    movedPassengers.insert(ent);
                    Vector2 push;
                    push.y = velocity.y - ((hit.distance - raycast->skinWidth) * directionY);
                    push.x = (directionY == -1) ? velocity.x : 0.0f;
                    movingPlatform->passengers.emplace_back(hit.target, push, directionY == -1, true);
                }
            }
        }
    }

    // horizontally moving platform
    if(velocity.x != 0) {
        float rayLength = (directionX == -1) ? velocity.x - raycast->skinWidth : velocity.x + raycast->skinWidth;
        for(i32 i = 0; i < raycast->horizontalRayCount; i++) {
            Vector2 rayOrigin = (directionX == -1) ? raycast->raycastOrigins.bottomLeft
                                                    : raycast->raycastOrigins.bottomRight;
            rayOrigin.y -= raycast->horizontalRaySpacing * i;
            Vector2 rayEnd(rayOrigin.x + rayLength, rayOrigin.y);

            RaycastHit hit;
            if (performRaycastActor(ent->getWorld(), rayOrigin, rayEnd, hit)) {
                if (movedPassengers.count(ent) == 0) {
                    movedPassengers.insert(ent);
                    Vector2 push;
                    push.y = 0;
                    push.x = velocity.x - ((hit.distance - raycast->skinWidth) * directionX);
                    movingPlatform->passengers.emplace_back(hit.target, push, false, true);
                }
            }
        }
    }

    // on top of horizontally or downward moving platform
    if(directionY == 1 || (velocity.y == 0 && velocity.x != 0)) {
        float rayLength = -2 * raycast->skinWidth;
        for(i32 i = 0; i < raycast->verticalRayCount; i++) {
            Vector2 rayOrigin =  raycast->raycastOrigins.topLeft;
            rayOrigin.x += raycast->verticalRaySpacing * i;
            Vector2 rayEnd(rayOrigin.x, rayOrigin.y + rayLength);

            RaycastHit hit;
            if(performRaycastActor(ent->getWorld(), rayOrigin, rayEnd, hit)) {
                if(movedPassengers.count(ent) == 0) {
                    movedPassengers.insert(ent);
                    Vector2 push;
                    push.y = velocity.y;
                    push.x = velocity.x;
                    movingPlatform->passengers.emplace_back(hit.target, push, true, false);
                }
            }
        }
    }
}


