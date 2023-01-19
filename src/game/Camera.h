//
// Created by bison on 19-12-22.
//

#ifndef PLATFORMER_CAMERA_H
#define PLATFORMER_CAMERA_H

#include <ECS.h>
#include <Vector2.h>
#include <SDL_log.h>
#include "components/InputComponent.h"

using namespace ECS;

struct FocusArea: public FloatRect {
    void focus(const FloatRect& targetBounds, const Vector2& size);
    void update(const FloatRect& targetBounds);
    Vector2 center;
    Vector2 velocity;
};

class Camera {
public:
    float levelWidth = 0;
    float levelHeight = 0;
    float scrollX = 0;
    float scrollY = 0;
    FocusArea focusArea;

    Camera(float levelWidth, float levelHeight) {
        setLevelDimensions(levelWidth, levelHeight);
    }

    void update(const FloatRect& targetBounds, float deltaTime, InputComponent& input, bool restrain);
    void update(const Vector2& target, float deltaTime);

    void setLevelDimensions(float w, float h) {
        levelWidth = w;
        levelHeight = h;
        maxScrollX = levelWidth - camWidth;
        maxScrollY = levelHeight - camHeight;
    }

    void focus(const FloatRect& targetBounds, const Vector2& size) {
        focusArea.focus(targetBounds, size);
        //focusArea.update(targetBounds);
        scrollX = roundf(focusArea.centerX() - (0.5f * camWidth));
        scrollY = roundf(focusArea.centerY() - (0.5f * camHeight));
        lookAheadDirX = 0;
        targetLookAheadX = 0;
        smoothLookVelocityX = 0;
        lookAheadStopped = false;
        targetX = 0;
        targetY = 0;
        targetXSmooth = 0;
        targetYSmooth = 0;
        currentLookAheadX = 0;
        focusArea.velocity.zero();
        scrollX = std::clamp(scrollX, 0.0f, maxScrollX);
        if (scrollY > maxScrollY) {
            scrollY = maxScrollY;
        }
        SDL_Log("ReFocus: %.2f,%.2f", scrollX, scrollY);
    }

private:
    float camWidth = 1920;
    float camHeight = 1080;
    float verticalSmoothTime = .2f;
    float targetXSmooth = 0;
    float targetYSmooth = 0;
    float maxScrollX = 0;
    float maxScrollY = 0;
    float targetX = 0;
    float targetY = 0;

    float lookAheadDstX = 200;
    float lookSmoothTimeX = 0.5f;
    float lookAheadDirX = 0;
    float currentLookAheadX = 0;
    float targetLookAheadX = 0;
    float smoothLookVelocityX = 0;
    bool lookAheadStopped = false;
};

#endif //PLATFORMER_CAMERA_H
