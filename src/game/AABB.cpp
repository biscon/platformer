//
// Created by bison on 25-11-22.
//

#include <cstring>
#include "AABB.h"

const float EPSILON = 1e-8;

float flabs(float value) {
    return value < 0 ? -value : value;
}

float fclamp(float value, float min, float max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

float fsign(float value) {
    return value < 0 ? -1 : 1;
}

AABB::AABB(FloatRect& rect) {
    center.x = rect.left + (rect.width()/2);
    center.y = rect.top + (rect.height()/2);
    half.x = rect.width()/2;
    half.y  = rect.height()/2;
}

bool AABB::intersectPos(const Vector2 &point, Hit &hit) {
    float dx = point.x - this->center.x;
    float px = this->half.x - flabs(dx);
    if (px <= 0) {
        return false;
    }

    float dy = point.y - this->center.y;
    float py = this->half.y - flabs(dy);
    if (py <= 0) {
        return false;
    }
    
    memset(&hit, 0, sizeof(Hit));
    hit.collider = this;
    if (px < py) {
        float sx = fsign(dx);
        hit.delta.x = px * sx;
        hit.normal.x = sx;
        hit.pos.x = this->center.x + this->half.x * sx;
        hit.pos.y = point.y;
    } else {
        float sy = fsign(dy);
        hit.delta.y = py * sy;
        hit.normal.y = sy;
        hit.pos.x = point.x;
        hit.pos.y = this->center.y + this->half.y * sy;
    }
    return true;
}

bool
AABB::intersectSegment(const Vector2 &pos, const Vector2 &delta, float paddingX, float paddingY,
                       Hit &hit) {
    float scaleX = 1.0f / delta.x;
    float scaleY = 1.0f / delta.y;
    float signX = fsign(scaleX);
    float signY = fsign(scaleY);
    float nearTimeX = (this->center.x - signX * (this->half.x + paddingX) - pos.x) * scaleX;
    float nearTimeY = (this->center.y - signY * (this->half.y + paddingY) - pos.y) * scaleY;
    float farTimeX = (this->center.x + signX * (this->half.x + paddingX) - pos.x) * scaleX;
    float farTimeY = (this->center.y + signY * (this->half.y + paddingY) - pos.y) * scaleY;
    if (nearTimeX > farTimeY || nearTimeY > farTimeX) {
        return false;
    }

    float nearTime = nearTimeX > nearTimeY ? nearTimeX : nearTimeY;
    float farTime = farTimeX < farTimeY ? farTimeX : farTimeY;
    if (nearTime >= 1 || farTime <= 0) {
        return false;
    }

    memset(&hit, 0, sizeof(Hit));
    hit.collider = this;
    hit.time = fclamp(nearTime, 0, 1);
    if (nearTimeX > nearTimeY) {
        hit.normal.x = -signX;
        hit.normal.y = 0;
    } else {
        hit.normal.x = 0;
        hit.normal.y = -signY;
    }
    hit.delta.x = (1.0f - hit.time) * -delta.x;
    hit.delta.y = (1.0f - hit.time) * -delta.y;
    hit.pos.x = pos.x + delta.x * hit.time;
    hit.pos.y = pos.y + delta.y * hit.time;
    return true;
}

bool AABB::intersectAABB(const AABB &box, Hit &hit) {
    float dx = box.center.x - this->center.x;
    float px = box.half.x + this->half.x - flabs(dx);
    if (px <= 0) {
        return false;
    }

    float dy = box.center.y - this->center.y;
    float py = box.half.y + this->half.y - flabs(dy);
    if (py <= 0) {
        return false;
    }

    memset(&hit, 0, sizeof(Hit));
    hit.collider = this;
    if (px < py) {
        float sx = fsign(dx);
        hit.delta.x = px * sx;
        hit.normal.x = sx;
        hit.pos.x = this->center.x + this->half.x * sx;
        hit.pos.y = box.center.y;
    } else {
        float sy = fsign(dy);
        hit.delta.y = py * sy;
        hit.normal.y = sy;
        hit.pos.x = box.center.x;
        hit.pos.y = this->center.y + this->half.y * sy;
    }
    return true;
}

void AABB::sweepAABB(const AABB& box, const Vector2 &delta, Sweep &sweep) {
    memset(&sweep, 0, sizeof(Sweep));

    if (delta.x == 0 && delta.y == 0) {
        sweep.pos.x = box.center.x;
        sweep.pos.y = box.center.y;
        sweep.didHit = intersectAABB(box, sweep.hit);
        sweep.time = sweep.didHit ? sweep.hit.time : 1;
        return;
    }

    if(intersectSegment(box.center, delta, box.half.x, box.half.y, sweep.hit)) {
        sweep.didHit = true;
        sweep.time = fclamp(sweep.hit.time - EPSILON, 0, 1);
        sweep.pos.x = box.center.x + delta.x * sweep.time;
        sweep.pos.y = box.center.y + delta.y * sweep.time;
        Vector2 direction = delta;
        direction.normalize();

        sweep.hit.pos.x = fclamp(
                sweep.hit.pos.x + direction.x * box.half.x,
                this->center.x - this->half.x,
                this->center.x + this->half.x
        );
        sweep.hit.pos.y = fclamp(
                sweep.hit.pos.y + direction.y * box.half.y,
                this->center.y - this->half.y,
                this->center.y + this->half.y
        );
    } else {
        sweep.didHit = false;
        sweep.pos.x = box.center.x + delta.x;
        sweep.pos.y = box.center.y + delta.y;
        sweep.time = 1;
    }
}

