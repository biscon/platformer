//
// Created by bison on 16-01-23.
//

#include "TransitionEffect.h"
#include "PolyUtil.h"
#include "Utils.h"
#include <algorithm>
#include <SDL_log.h>

TransitionEffect::TransitionEffect(const RenderBuffers &buffers, LevelConfig &config) : buffers(buffers),
                                                                                        config(config) {
    buildTorusTriangleStripMesh(Vector2(100,100), 20, 100, segments, innerColor, outerColor, mesh);
    buildTorusTriangleStripMesh(Vector2(100,100), 20, 100, segments, innerColor, outerColor, fadeMesh);
}

void TransitionEffect::start(TransitionState state, const Vector2& focus) {
    this->state = state;
    timer = duration;
    isDone = false;
    this->focus = focus;
    outerRadius = 200.0f + config.screenWidth;
    innerRadius = 0.0f;
    currentRadius = 500;
    updateTorusTriangleStripMesh(focus, currentRadius, outerRadius, segments, innerColor, outerColor, mesh);
    updateTorusTriangleStripMesh(focus, currentRadius - fadeMeshRadius, currentRadius, segments, TRANSPARENT, innerColor, fadeMesh);
}

void TransitionEffect::update(float deltaTime) {
    if(!isDone) {
        if (timer > 0) {
            timer -= deltaTime;
        } else {
            isDone = true;
            return;
        }

        float currentFadeRadius = 0;
        switch(state) {
            case TransitionState::In: {
                float fraction = timer / duration;
                fraction = easeInSine(fraction);
                currentRadius = glm_lerp(outerRadius, innerRadius, fraction);
                currentFadeRadius = currentRadius - fadeMeshRadius;
                currentFadeRadius = std::clamp(currentFadeRadius, 0.0f, outerRadius);
                break;
            }
            case TransitionState::Out: {
                float fraction = 1 - (timer / duration);
                fraction = easeOutSine(fraction);
                currentRadius = glm_lerp(outerRadius, innerRadius, fraction);
                currentFadeRadius = currentRadius - fadeMeshRadius;
                currentFadeRadius = std::clamp(currentFadeRadius, 0.0f, outerRadius);
                break;
            }
        }

        updateTorusTriangleStripMesh(focus, currentRadius, outerRadius, segments, innerColor, outerColor, mesh);
        updateTorusTriangleStripMesh(focus, currentFadeRadius, currentRadius, segments, TRANSPARENT, innerColor, fadeMesh);
    }
}

void TransitionEffect::render() {
    buffers.unlit.pushTriangleStripMesh(mesh);
    buffers.unlit.pushTriangleStripMesh(fadeMesh);
}

