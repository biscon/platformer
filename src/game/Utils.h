//
// Created by bison on 25-11-22.
//

#ifndef GAME_MOVINGSYSTEM_H
#define GAME_MOVINGSYSTEM_H

#include <ECS.h>
#include <Vector2.h>

using namespace ECS;

void getBounds(Entity* ent, FloatRect& bounds);
void rectToPolygon(const FloatRect& rect, std::vector<Vector2>& polygon);
void rebuildMovingPlatform(Entity *ent);
float ease(float x, float a);
float easeOutSine(float x);
float easeInSine(float x);
float easeOutCubic(float x);
float easeInCubic(float x);


#endif //GAME_MOVINGSYSTEM_H
