//
// Created by bison on 25-11-22.
//

#include <SDL_log.h>
#include "Utils.h"
#include "components/ActorComponent.h"
#include "components/TerrainComponent.h"
#include "components/PlatformComponent.h"
#include "components/TransformComponent.h"
#include "components/SpriteComponent.h"
#include "PolyUtil.h"

void getBounds(Entity* ent, FloatRect& bounds) {
    auto sprite = ent->get<SpriteComponent>();
    auto transform = ent->get<TransformComponent>();

    if(!sprite.isValid() || !transform.isValid()) {
        return;
    }
    
    FloatRect rect;
    sprite->getBoundingBox(&rect);
    rect.left *= transform->scale;
    rect.right *= transform->scale;
    if(sprite->vertFlip) {
        rect.top *= -transform->scale;
        rect.bottom *= -transform->scale;
    } else {
        rect.top *= transform->scale;
        rect.bottom *= transform->scale;
    }

    // translate to world space
    float ox,oy;
    sprite->getOrigin(&ox, &oy, transform->scale);
    float x = transform->pos.x - ox;
    float y = transform->pos.y - oy;

    rect.left += x;
    rect.right += x;
    rect.top += y;
    rect.bottom += y;
    bounds = rect;
}

void rectToPolygon(const FloatRect& rect, std::vector<Vector2>& polygon) {
    if(polygon.empty()) {
        for(i32 i = 0; i < 4; ++i) {
            polygon.emplace_back(Vector2(0,0));
        }
    }
    polygon[0].x = rect.left;
    polygon[0].y = rect.top;
    polygon[1].x = rect.right;
    polygon[1].y = rect.top;
    polygon[2].x = rect.right;
    polygon[2].y = rect.bottom;
    polygon[3].x = rect.left;
    polygon[3].y = rect.bottom;

}

void rebuildMovingPlatform(Entity *ent) {
    auto movingPlatform = ent->get<PlatformComponent>();
    auto terrain = ent->get<TerrainComponent>();
    auto transform = ent->get<TransformComponent>();
    if(!movingPlatform.isValid() || !terrain.isValid() || !transform.isValid()) {
        return;
    }
    if(terrain->polygon.empty()) {
        for(i32 i = 0; i < 4; ++i) {
            terrain->polygon.emplace_back(Vector2(0,0));
        }
    }
    terrain->polygon[0].x = transform->pos.x;
    terrain->polygon[0].y = transform->pos.y;
    terrain->polygon[1].x = transform->pos.x + movingPlatform->size.x;
    terrain->polygon[1].y = transform->pos.y;
    terrain->polygon[2].x = transform->pos.x + movingPlatform->size.x;
    terrain->polygon[2].y = transform->pos.y + movingPlatform->size.y;
    terrain->polygon[3].x = transform->pos.x;
    terrain->polygon[3].y = transform->pos.y + movingPlatform->size.y;

    terrain->rebuildMesh();
    //terrain->show = true;
}

float ease(float x, float a) {
    a++;
    return powf(x,a) / (powf(x,a) + powf(1-x, a));
}

float easeOutSine(float x) {
    return sinf((x * PI) / 2);
}

float easeInSine(float x) {
    return 1.0f - cosf((x * PI) / 2.0f);
}

float easeInCubic(float x) {
    return x * x * x;
}

float easeOutCubic(float x) {
    return 1.0f - powf(1.0f - x, 3.0f);
}