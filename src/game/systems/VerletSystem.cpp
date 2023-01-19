//
// Created by bison on 08-01-23.
//
#include <algorithm>
#include <random>
#include "VerletSystem.h"
#include "../components/TerrainComponent.h"
#include "../components/PlatformComponent.h"
#include "../components/TransformComponent.h"

void VerletSystem::tick(World *world, float deltaTime) {
    for (Entity *ent : world->all(false)) {
        auto verlet = ent->get<VerletMeshComponent>();
        auto transform = ent->get<TransformComponent>();
        auto platform = ent->get<PlatformComponent>();
        if(transform.isValid() && verlet.isValid()) {
            updateVerletMesh(verlet.get(), deltaTime);
            if(platform.isValid()) {
                Vector2 curPos(transform->pos.x, transform->pos.y);
                curPos += verlet->attachOffset;
                Vector2 offset = verlet->points.back().position - curPos;
                platform->moveRequest = Vector2(offset.x, offset.y);
            } else {
                transform->pos.x = verlet->points.back().position.x - verlet->attachOffset.x;
                transform->pos.y = verlet->points.back().position.y - verlet->attachOffset.y;
            }
        }
    }
}

INTERNAL auto rd = std::random_device {};
INTERNAL auto rng = std::default_random_engine { rd() };

void VerletSystem::updateVerletMesh(VerletMeshComponent& verlet, float deltaTime) {
    for(auto& p : verlet.points) {
        if(!p.locked) {
            Vector2 posBeforeUpdate = p.position;
            p.position += p.position - p.prevPosition;
            p.position.y += verlet.gravity * deltaTime * deltaTime;
            p.prevPosition = posBeforeUpdate;
        }
    }

    std::shuffle(verlet.order.begin(), verlet.order.end(), rng);
    for(u32 i = 0; i < verlet.solveIterations; i++) {
        for(u32 s = 0; s < verlet.sticks.size(); s++) {
            VerletStick& stick = verlet.sticks[verlet.order[s]];
            VerletPoint& pointA = verlet.points[stick.indexA];
            VerletPoint& pointB = verlet.points[stick.indexB];
            //VerletStick& stick = verlet.sticks[s];
            Vector2 stickCenter = (pointA.position + pointB.position) / 2;
            Vector2 stickDir = (pointA.position - pointB.position).normalized();
            float length = (pointA.position - pointB.position).length();
            if(length > stick.length || stick.lockLength) {
                if(!pointA.locked) {
                    pointA.position = stickCenter + stickDir * stick.length / 2;
                }
                if(!pointB.locked) {
                    pointB.position = stickCenter - stickDir * stick.length / 2;
                }
            }
        }
    }
    u32 i = (u32) verlet.points.size()-5;
    verlet.topLeft = verlet.points[i+1].position;
    verlet.topRight = verlet.points[i+2].position;
    verlet.bottomLeft = verlet.points[i+3].position;
    verlet.bottomRight = verlet.points[i+4].position;
    /*
    Vector2 windDir(1,0);
    for(u64 i = verlet.points.size() - (verlet.points.size()/6); i < verlet.points.size(); i++) {
        auto& p = verlet.points[i];
        if(!p.locked) {
            p.position += windDir * 500.0f * deltaTime * deltaTime;
        }
    }
     */
}
