//
// Created by bison on 01-01-23.
//

#include "RaycastSystem.h"
#include "../components/TerrainComponent.h"
#include "../PolyUtil.h"
#include "../Utils.h"
#include "../components/LadderComponent.h"

bool RaycastSystem::performRaycast(World *world, Vector2& rayOrigin, Vector2& rayEnd, RaycastHit& hit, bool checkLadders) {
    RaycastHit result;
    result.distance = INFINITY;

    for (Entity *ent : world->all(false)) {
        auto terrain = ent->get<TerrainComponent>();
        auto ladder = ent->get<LadderComponent>();
        if(terrain.isValid()) {
            RaycastHit tmpHit;
            if(polyLineIntersection(terrain->polygon, rayOrigin, rayEnd, tmpHit.pos.x, tmpHit.pos.y, tmpHit.normal)) {
                tmpHit.distance = distance(rayOrigin, tmpHit.pos);
                tmpHit.target = ent;
                if(tmpHit.distance < result.distance) {
                    result = tmpHit;
                }
            }
        }
        // check top of ladder
        if(ladder.isValid() && checkLadders) {
            Vector2 lineStart(ladder->rect.left, ladder->rect.top);
            Vector2 lineEnd(ladder->rect.right, ladder->rect.top);
            RaycastHit tmpHit;
            if(lineLineIntersection(lineStart, lineEnd, rayOrigin, rayEnd, tmpHit.pos.x, tmpHit.pos.y, tmpHit.normal)) {
                tmpHit.distance = distance(rayOrigin, tmpHit.pos);
                tmpHit.target = ent;
                if(tmpHit.distance < result.distance) {
                    result = tmpHit;
                }
            }
        }

    }
    if(result.distance != INFINITY) {
        hit = result;
        return true;
    }
    return false;
}

bool RaycastSystem::performRaycastLadder(World *world, Vector2& rayOrigin, Vector2& rayEnd, RaycastHit& hit) {
    RaycastHit result;
    result.distance = INFINITY;
    for (Entity *ent : world->all(false)) {
        auto ladder = ent->get<LadderComponent>();
        // check top of ladder
        if(ladder.isValid()) {
            Vector2 lineStart(ladder->rect.left, ladder->rect.top);
            Vector2 lineEnd(ladder->rect.right, ladder->rect.top);
            RaycastHit tmpHit;
            if(lineLineIntersection(lineStart, lineEnd, rayOrigin, rayEnd, tmpHit.pos.x, tmpHit.pos.y, tmpHit.normal)) {
                tmpHit.distance = distance(rayOrigin, tmpHit.pos);
                tmpHit.target = ent;
                if(tmpHit.distance < result.distance) {
                    result = tmpHit;
                }
            }
        }

    }
    if(result.distance != INFINITY) {
        hit = result;
        return true;
    }
    return false;
}


bool RaycastSystem::performRaycastActor(World *world, Vector2& rayOrigin, Vector2& rayEnd, RaycastHit& hit) {
    RaycastHit result;
    result.distance = INFINITY;
    for (Entity *ent : world->all(false)) {
        auto actor = ent->get<ActorComponent>();
        if(actor.isValid()) {
            FloatRect bounds;
            bounds.zero();
            getBounds(ent, bounds);

            std::vector<Vector2> polygon;
            rectToPolygon(bounds, polygon);

            RaycastHit tmpHit;
            if(polyLineIntersection(polygon, rayOrigin, rayEnd, tmpHit.pos.x, tmpHit.pos.y, tmpHit.normal)) {
                tmpHit.distance = distance(rayOrigin, tmpHit.pos);
                tmpHit.target = ent;
                if(tmpHit.distance < result.distance) {
                    result = tmpHit;
                }
            }
        }

    }
    if(result.distance != INFINITY) {
        hit = result;
        return true;
    }
    return false;
}


void RaycastSystem::updateRaycastOrigins(CollisionComponent &raycast, FloatRect b) {
    b.expand(raycast.skinWidth * -1);
    raycast.raycastOrigins.topLeft.set(b.left, b.top);
    raycast.raycastOrigins.topRight.set(b.right, b.top);
    raycast.raycastOrigins.bottomLeft.set(b.left, b.bottom);
    raycast.raycastOrigins.bottomRight.set(b.right, b.bottom);
}

void RaycastSystem::calculateRaySpacing(CollisionComponent &raycast, FloatRect b) {
    b.expand(raycast.skinWidth * -1);

    raycast.horizontalRayCount = std::clamp(raycast.horizontalRayCount, 2, INT_MAX);
    raycast.verticalRayCount = std::clamp(raycast.verticalRayCount, 2, INT_MAX);

    raycast.horizontalRaySpacing = b.height() / (raycast.horizontalRayCount - 1);
    raycast.verticalRaySpacing = b.width() / (raycast.verticalRayCount - 1);
    //SDL_Log("b.width() = %.2f, verticalRayCount = %d", b.width(), raycast.verticalRayCount);
    //buffer.pushRect(b, YELLOW);
    //buffer.pushLine(Vector2(b.left, b.bottom), Vector2(b.right, b.bottom), GREEN)
}
