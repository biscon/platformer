//
// Created by bison on 13-12-22.
//

#include <climits>
#include <SDL_log.h>
#include <Vector2.h>
#include "CollisionSystem.h"
#include "../components/TerrainComponent.h"
#include "../PolyUtil.h"
#include "../components/ActorComponent.h"
#include "../Utils.h"
#include "../components/TransformComponent.h"
#include "../components/PhysicComponent.h"
#include "../components/LadderComponent.h"
#include "../AABB.h"
#include "../components/DoorComponent.h"

CollisionSystem::CollisionSystem(RenderCmdBuffer &buffer): buffer(buffer) {
}

void CollisionSystem::tick(World *world, float deltaTime) {

    // move platforms and passengers
    for (Entity *ent : world->all(false)) {
        auto movingPlatform = ent->get<PlatformComponent>();
        auto transform = ent->get<TransformComponent>();
        if(movingPlatform.isValid() && transform.isValid()) {
            movePassengers(movingPlatform.get(), true);
            transform->pos += movingPlatform->velocity;
            rebuildMovingPlatform(ent);
            movePassengers(movingPlatform.get(), false);
        }
    }

    // move actors
    for (Entity *ent : world->all(false)) {
        auto physic = ent->get<PhysicComponent>();
        auto input = ent->get<InputComponent>();
        auto colInfo = ent->get<CollisionInfoComponent>();
        auto actor = ent->get<ActorComponent>();

        if(!physic.isValid() || !input.isValid() || !colInfo.isValid() || !actor.isValid()) {
            continue;
        }

        if(actor->state != ActorState::Ladder) {
            physic->velocity.y += physic->gravity * deltaTime;
        }

        Vector2 v = physic->velocity;
        v *= deltaTime;

        move(ent, v);

        if(colInfo->cur().above || colInfo->cur().below) {
            if(!colInfo->cur().slidingDownMaxSlope) {
                physic->velocity.y = 0;
            }
        }

        colInfo->cur().ascending = physic->velocity.y < 0;
        senseLadder(ent);
        senseTopOfLadder(ent);
    }

}

void CollisionSystem::move(Entity* ent, Vector2 velocity) {
    auto transform = ent->get<TransformComponent>();
    auto raycast = ent->get<CollisionComponent>();
    auto colInfo = ent->get<CollisionInfoComponent>();
    auto actor = ent->get<ActorComponent>();
    auto input = ent->get<InputComponent>();
    if(!transform.isValid() || !raycast.isValid() || !colInfo.isValid() || !actor.isValid() || !input.isValid()) {
        return;
    }
    FloatRect bounds;
    bounds.zero();
    getBounds(ent, bounds);
    calculateRaySpacing(raycast.get(), bounds);
    updateRaycastOrigins(raycast.get(), bounds);

    // store previous frame collision info & reset
    colInfo->info[1] = colInfo->info[0];
    colInfo->cur().reset();

    //SDL_Log("-------------------------------------------------------------");
    //SDL_Log("precol vel = %.2f,%.2f", velocity.x, velocity.y);

    if(velocity.y > 0) {
        descendSlope(ent->getWorld(), raycast.get(), colInfo.get(), velocity);
    }

    if(velocity.x != 0) {
        horizontalCollisions(ent->getWorld(), raycast.get(), colInfo.get(), velocity);
    }
    if(velocity.y != 0) {
        verticalCollisions(ent->getWorld(), raycast.get(), colInfo.get(), actor.get(), input.get(), velocity);
    }

    findDistanceGround(ent->getWorld(), raycast.get(), colInfo.get(), velocity);
    //SDL_Log("applying vel = %.2f,%.2f", velocity.x, velocity.y);

    transform->pos.x += velocity.x;
    transform->pos.y += velocity.y;

    colInfo->cur().velocity = velocity;
    //SDL_Log("pos.y = %.2f vel.y = %.2f", transform->pos.y, velocity.y);
    //transform->pos.round();
}

void CollisionSystem::horizontalCollisions(World* world, CollisionComponent& raycast, CollisionInfoComponent& colInfo, Vector2& velocity) {
    float directionX = glm_signf(velocity.x);
    float rayLength = (directionX == -1) ? velocity.x - raycast.skinWidth : velocity.x + raycast.skinWidth;
    //rayLength *= 2;

    for(i32 i = 0; i < raycast.horizontalRayCount; i++) {
        Vector2 rayOrigin = (directionX == -1) ? raycast.raycastOrigins.bottomLeft : raycast.raycastOrigins.bottomRight;
        rayOrigin.y -= raycast.horizontalRaySpacing * (float) i;
        Vector2 rayEnd(rayOrigin.x + rayLength, rayOrigin.y);
        //buffer.pushLine(rayOrigin, rayEnd, Renderer::RED);

        RaycastHit hit;
        if(performRaycast(world, rayOrigin, rayEnd, hit, false)) {
            if(hit.distance == 0) {
                continue;
            }

            auto terrain = hit.target->get<TerrainComponent>();
            if(terrain.isValid()) {
                if(pointInPolygon(rayOrigin, terrain->polygon)) {
                    //SDL_Log("ray originate inside same poly, skip");
                    continue;
                }
            }

            if(colInfo.cur().descendingSlope) {
                colInfo.cur().descendingSlope = false;
                velocity = colInfo.prev().velocity;
            }
            float slopeAngle = angleBetweenVectors(hit.normal, Vector2(0, -1));
            float degrees = radiansToDegrees(slopeAngle);
            if(i == 0 && degrees <= raycast.maxSlopeAngle) {
                //SDL_Log("climbing slopeAngle: %.2f", degrees);
                float distanceToSlopeStart = 0;
                if(slopeAngle != colInfo.prev().slopeAngle) {
                    distanceToSlopeStart = hit.distance - raycast.skinWidth;
                    velocity.x -= distanceToSlopeStart * directionX;
                    SDL_Log("distanceToSlopeStart: %.2f", distanceToSlopeStart);
                }
                climbSlope(velocity, slopeAngle, colInfo);
                velocity.x += distanceToSlopeStart * directionX;
            }
            if(!colInfo.cur().climbingSlope || degrees > raycast.maxSlopeAngle) {
                //SDL_Log("Correcting");
                velocity.x = (hit.distance - raycast.skinWidth) * directionX;
                rayLength = hit.distance;

                if(colInfo.cur().climbingSlope) {
                    velocity.y = tanf(colInfo.cur().slopeAngle) * fabsf(velocity.x);
                }

                colInfo.cur().left = directionX == -1;
                colInfo.cur().right = directionX == 1;
            }
        }
    }

}

void CollisionSystem::verticalCollisions(World* world, CollisionComponent& raycast, CollisionInfoComponent& colInfo, ActorComponent& actor, InputComponent& input, Vector2& velocity) {
    float directionY = glm_signf(velocity.y);
    float rayLength = (directionY == -1) ? velocity.y - raycast.skinWidth : velocity.y + raycast.skinWidth;
    //rayLength *= 5;

    for(i32 i = 0; i < raycast.verticalRayCount; i++) {
        Vector2 rayOrigin = (directionY == 1) ? raycast.raycastOrigins.bottomLeft : raycast.raycastOrigins.topLeft;
        rayOrigin.x += raycast.verticalRaySpacing * i + velocity.x;
        Vector2 rayEnd(rayOrigin.x, rayOrigin.y + rayLength);
        //buffer.pushLine(rayOrigin, rayEnd, Renderer::RED);

        RaycastHit hit;
        bool checkLadders = directionY == 1 && actor.state != ActorState::Ladder;
        if(performRaycast(world, rayOrigin, rayEnd, hit, checkLadders)) {
            auto terrain = hit.target->get<TerrainComponent>();
            if(terrain.isValid()) {
                if(pointInPolygon(rayOrigin, terrain->polygon)) {
                    //SDL_Log("ray originate inside same poly, skip");
                    continue;
                }
                if(terrain->through) {
                    if(directionY == -1 || hit.distance == 0) {
                        continue;
                    }
                    if(input.cur().down) {
                        continue;
                    }
                }
            }

            velocity.y = (hit.distance - raycast.skinWidth) * directionY;
            rayLength = hit.distance;

            if(colInfo.cur().climbingSlope) {
                velocity.x = velocity.y / tanf(colInfo.cur().slopeAngle) * glm_signf(velocity.x);
            }

            colInfo.cur().below = directionY == 1;
            colInfo.cur().above = directionY == -1;
        }
    }
    if(colInfo.cur().climbingSlope) {
        float directionX = glm_signf(velocity.x);
        //rayLength = fabsf(velocity.x) + raycast.skinWidth;
        rayLength = (directionX == -1) ? velocity.x - raycast.skinWidth : velocity.x + raycast.skinWidth;
        //rayLength *= 8;
        Vector2 rayOrigin = ((directionX == -1) ? raycast.raycastOrigins.bottomLeft : raycast.raycastOrigins.bottomRight);
        rayOrigin.y += velocity.y;
        Vector2 rayEnd(rayOrigin.x + rayLength, rayOrigin.y);
        //buffer.pushLine(rayOrigin, rayEnd, Renderer::YELLOW);

        RaycastHit hit;
        if(performRaycast(world, rayOrigin, rayEnd, hit, false)) {
            float slopeAngle = angleBetweenVectors(hit.normal, Vector2(0, -1));
            if(slopeAngle != colInfo.cur().slopeAngle) {
                SDL_Log("new slope angle while climbing slope: %.2f", radiansToDegrees(slopeAngle));
                velocity.x = (hit.distance - raycast.skinWidth) * directionX;
                colInfo.cur().slopeAngle = slopeAngle;
            }
        }
    }
}

void CollisionSystem::climbSlope(Vector2& velocity, float slopeAngle, CollisionInfoComponent& colInfo) {
    float moveDistance = fabsf(velocity.x);
    float climbVelocityY = sinf(slopeAngle) * -moveDistance;
    if(climbVelocityY <= velocity.y) {
        velocity.y = climbVelocityY;
        velocity.x = cosf(slopeAngle) * moveDistance * glm_signf(velocity.x);
        colInfo.cur().below = true;
        colInfo.cur().climbingSlope = true;
        colInfo.cur().slopeAngle = slopeAngle;
    } else {
        SDL_Log("Jumping on slope climbVelocityY: %.2f <= velocity.y: %.2f", climbVelocityY, velocity.y);
    }
}

void CollisionSystem::descendSlope(World* world, CollisionComponent& raycast, CollisionInfoComponent& colInfo, Vector2& velocity) {

    RaycastHit maxSlopeHitLeft;
    Vector2 leftRayOrigin = raycast.raycastOrigins.bottomLeft;
    Vector2 leftRayEnd(leftRayOrigin.x, leftRayOrigin.y + fabsf(velocity.y) + raycast.skinWidth);
    bool hitLeft = performRaycast(world, leftRayOrigin, leftRayEnd, maxSlopeHitLeft, false);

    RaycastHit maxSlopeHitRight;
    Vector2 rightRayOrigin = raycast.raycastOrigins.bottomRight;
    Vector2 rightRayEnd(rightRayOrigin.x, rightRayOrigin.y + fabsf(velocity.y) + raycast.skinWidth);
    bool hitRight = performRaycast(world, rightRayOrigin, rightRayEnd, maxSlopeHitRight, false);

    if(hitLeft != hitRight) {
        if (hitLeft) {
            slideDownMaxSlope(maxSlopeHitLeft, velocity, raycast.maxSlopeAngle, colInfo);
        }
        if (hitRight) {
            slideDownMaxSlope(maxSlopeHitRight, velocity, raycast.maxSlopeAngle, colInfo);
        }
    }

    if(!colInfo.cur().slidingDownMaxSlope) {
        float directionX = glm_signf(velocity.x);
        Vector2 rayOrigin = (directionX == -1) ? raycast.raycastOrigins.bottomRight
                                                : raycast.raycastOrigins.bottomLeft;
        Vector2 rayEnd(rayOrigin.x, rayOrigin.y + 500);

        //buffer.pushLine(rayOrigin, rayEnd, Renderer::GREEN);

        RaycastHit hit;
        if (performRaycast(world, rayOrigin, rayEnd, hit, false)) {
            auto terrain = hit.target->get<TerrainComponent>();
            if (terrain.isValid()) {
                if (pointInPolygon(rayOrigin, terrain->polygon)) {
                    //SDL_Log("ray originate inside same poly, skip");
                    return;
                }
            }
            float slopeAngle = angleBetweenVectors(hit.normal, Vector2(0, -1));
            if (slopeAngle != 0 && radiansToDegrees(slopeAngle) <= raycast.maxSlopeAngle) {
                if (glm_signf(hit.normal.x) == directionX) {
                    if (hit.distance - raycast.skinWidth <= tanf(slopeAngle) * fabsf(velocity.x)) {
                        //SDL_Log("detected downwards slope angle: %.2f", radiansToDegrees(slopeAngle));
                        float moveDistance = fabsf(velocity.x);
                        float descendVelocityY = sinf(slopeAngle) * moveDistance;
                        velocity.x = cosf(slopeAngle) * moveDistance * glm_signf(velocity.x);
                        velocity.y += descendVelocityY;

                        colInfo.cur().slopeAngle = slopeAngle;
                        colInfo.cur().descendingSlope = true;
                        colInfo.cur().below = true;
                    }
                }
            }
        }
    }
}

void CollisionSystem::slideDownMaxSlope(const RaycastHit& hit, Vector2& velocity, float maxSlopeAngle, CollisionInfoComponent& colInfo) {
    float slopeAngle = angleBetweenVectors(hit.normal, Vector2(0, -1));
    //SDL_Log("SlopeAngle: %f, normal: %.2f,%.2f", slopeAngle, hit.normal.x, hit.normal.y);
    if(radiansToDegrees(slopeAngle) > maxSlopeAngle) {
        velocity.x = hit.normal.x * (fabs(velocity.y) - hit.distance) / tanf(slopeAngle);
        colInfo.cur().slopeAngle = slopeAngle;
        colInfo.cur().slidingDownMaxSlope = true;
        colInfo.cur().slopeNormal = hit.normal;
    }
}

void CollisionSystem::findDistanceGround(World* world, CollisionComponent& raycast, CollisionInfoComponent& colInfo, Vector2& velocity) {
    float rayLength = 200;
    colInfo.cur().distanceGround = rayLength;

    for(i32 i = 0; i < raycast.verticalRayCount; i++) {
        Vector2 rayOrigin = raycast.raycastOrigins.bottomLeft;
        //rayOrigin.x += raycast.verticalRaySpacing * i + velocity.x;
        rayOrigin.x += raycast.verticalRaySpacing * i;
        Vector2 rayEnd(rayOrigin.x, rayOrigin.y + rayLength);
        //buffer.pushLine(rayOrigin, rayEnd, Renderer::YELLOW);
        RaycastHit hit;
        if(performRaycast(world, rayOrigin, rayEnd, hit, true)) {
            //buffer.pushLine(rayOrigin, hit.pos, Renderer::GREEN);
            if(hit.distance < colInfo.cur().distanceGround) {
                colInfo.cur().distanceGround = hit.distance - raycast.skinWidth;
            }
        }
    }
}

void CollisionSystem::senseLadder(Entity *ent) {
    auto colInfo = ent->get<CollisionInfoComponent>();
    if(!colInfo.isValid()) {
        return;
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

    for (Entity* ladderEnt : ent->getWorld()->each<LadderComponent>()) {
        auto ladder = ladderEnt->get<LadderComponent>();
        AABB ladderAABB(ladder->rect);
        Hit hit;
        if(midSensor.intersectAABB(ladderAABB, hit)) {
            colInfo->cur().onLadder = true;
            colInfo->cur().distanceLadderCenter = entAABB.center.x - ladderAABB.center.x;
        }
    }
}

void CollisionSystem::senseTopOfLadder(Entity *ent) {
    auto colInfo = ent->get<CollisionInfoComponent>();
    auto collision = ent->get<CollisionComponent>();
    if(!colInfo.isValid() || !collision.isValid()) {
        return;
    }

    float rayLength = 10;

    Vector2 leftRayStart = collision->raycastOrigins.bottomLeft;
    Vector2 leftRayEnd = leftRayStart;
    leftRayEnd.y += rayLength;
    Vector2 rightRayStart = collision->raycastOrigins.bottomRight;
    Vector2 rightRayEnd = rightRayStart;
    rightRayEnd.y += rayLength;

    bool hitLeft = false;
    bool hitRight = false;

    RaycastHit hit;
    if(performRaycastLadder(ent->getWorld(), leftRayStart, leftRayEnd, hit)) {
        if(hit.distance - collision->skinWidth == 0) {
            hitLeft = true;
        }
    }
    if(performRaycastLadder(ent->getWorld(), rightRayStart, rightRayEnd, hit)) {
        if(hit.distance - collision->skinWidth == 0) {
            hitRight = true;
        }
    }
    if(hitLeft && hitRight) {
        colInfo->cur().onTopLadder = true;
    }
}

void CollisionSystem::movePassengers(PlatformComponent& movingPlatform, bool moveBeforePlatform) {
    for(Passenger& p : movingPlatform.passengers) {
        if(p.moveBeforePlatform == moveBeforePlatform) {
            move(p.entity, p.velocity);
        }
    }
}


