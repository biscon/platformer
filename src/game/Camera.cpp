//
// Created by bison on 19-12-22.
//

#include "Camera.h"
#include "PolyUtil.h"

#include <algorithm>
#include <SDL_log.h>
#include <cglm/util.h>

void FocusArea::focus(const FloatRect& targetBounds, const Vector2& size) {
    left = targetBounds.centerX() - size.x/2;
    right = targetBounds.centerX() + size.x/2;
    bottom = targetBounds.bottom;
    top = bottom - size.y;
    center.x = (left + right) / 2;
    center.y = (top + bottom) / 2;
    round();
}

void FocusArea::update(const FloatRect &targetBounds) {
    float shiftX = 0;
    if(targetBounds.left < left) {
        shiftX = targetBounds.left - left;
    } else if(targetBounds.right > right) {
        shiftX = targetBounds.right - right;
    }
    left += shiftX;
    right += shiftX;


    float shiftY = 0;
    if(targetBounds.bottom > bottom) {
        shiftY = targetBounds.bottom - bottom;
    } else if(targetBounds.top < top) {
        shiftY = targetBounds.top - top;
    }
    top += shiftY;
    bottom += shiftY;
    center.x = (left + right) / 2;
    center.y = (top + bottom) / 2;
    velocity.x = shiftX;
    velocity.y = shiftY;
    round();
}

void Camera::update(const FloatRect& targetBounds, float deltaTime,  InputComponent& input, bool restrain) {
    focusArea.update(targetBounds);

    targetX = focusArea.center.x;
    targetY = focusArea.center.y;
    targetY = smoothDamp(targetY, focusArea.center.y, targetYSmooth, verticalSmoothTime, INFINITY, deltaTime);

    if(focusArea.velocity.x != 0) {
        lookAheadDirX = glm_signf(focusArea.velocity.x);
        if(input.cur().right && lookAheadDirX == 1) {
            lookAheadStopped = false;
            targetLookAheadX = lookAheadDirX * lookAheadDstX;
        } else if(input.cur().left && lookAheadDirX == -1) {
            lookAheadStopped = false;
            targetLookAheadX = lookAheadDirX * lookAheadDstX;
        } else {
            if(!lookAheadStopped) {
                lookAheadStopped = true;
                targetLookAheadX = currentLookAheadX + (lookAheadDirX * lookAheadDstX - currentLookAheadX) / 4.0f;
            }
        }
    }

    currentLookAheadX = smoothDamp(currentLookAheadX, targetLookAheadX, smoothLookVelocityX, lookSmoothTimeX, INFINITY, deltaTime);

    targetX += currentLookAheadX;

    scrollX = roundf(targetX - (0.5f * camWidth));
    scrollY = roundf(targetY - (0.5f * camHeight));

    if(restrain) {
        scrollX = std::clamp(scrollX, 0.0f, maxScrollX);
        if (scrollY > maxScrollY) {
            scrollY = maxScrollY;
        }
    }
}

void Camera::update(const Vector2 &target, float deltaTime) {
    targetX = smoothDamp(targetX, target.x, targetXSmooth, verticalSmoothTime, INFINITY, deltaTime);
    targetY = smoothDamp(targetY, target.y, targetYSmooth, verticalSmoothTime, INFINITY, deltaTime);
    scrollX = roundf(targetX - (0.5f * camWidth));
    scrollY = roundf(targetY - (0.5f * camHeight));
}
