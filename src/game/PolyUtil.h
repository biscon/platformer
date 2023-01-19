//
// Created by bison on 12-12-22.
//

#ifndef PLATFORMER_POLYUTIL_H
#define PLATFORMER_POLYUTIL_H

#include <vector>
#include <Vector2.h>
#include "../renderer/Types.h"

#define PI 3.1415926535897f

using namespace Renderer;

bool pointInPolygon(Vector2 point, const std::vector<Vector2> &points);
bool pointInCircle(const Vector2& point, const Vector2& pos, float radius);
bool polyLineIntersection(const std::vector<Vector2> &polygon, Vector2 rayStart, Vector2 rayEnd, float& x, float& y, Vector2& normal);
bool lineLineIntersection(Vector2 lineStart, Vector2 lineEnd, Vector2 rayStart, Vector2 rayEnd,
                          float &x, float &y, Vector2& normal);
float distance(const Vector2& a, const Vector2& b);
float angleBetweenVectors(const Vector2& a, const Vector2& b);
float radiansToDegrees(float radians);
float smoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
void buildTorusTriangleStripMesh(const Vector2& translate, float inner, float outer, u32 pts, const Color& innerColor, const Color& outerColor, std::vector<float>& vertices);
void updateTorusTriangleStripMesh(const Vector2& translate, float inner, float outer, u32 pts, const Color& innerColor, const Color& outerColor, std::vector<float>& vertices);
void extendLine(Vector2& p1, Vector2& p2, float amount);
void extendLineDirection(Vector2& p1, Vector2& p2, float amount);
float randomFloatRange(float min, float max);

#endif //PLATFORMER_POLYUTIL_H
