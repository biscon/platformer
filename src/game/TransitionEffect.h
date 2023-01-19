//
// Created by bison on 16-01-23.
//

#ifndef PLATFORMER_TRANSITIONEFFECT_H
#define PLATFORMER_TRANSITIONEFFECT_H


#include <RenderBuffers.h>
#include <LevelConfig.h>
#include "../renderer/RenderCmdBuffer.h"

using namespace Renderer;

enum class TransitionState {
    In,
    Out
};

class TransitionEffect {
public:
    TransitionEffect(const RenderBuffers &buffers, LevelConfig &config);

    void start(TransitionState state, const Vector2& focus);
    void update(float deltaTime);
    void render();

    TransitionState state = TransitionState::In;
    bool isDone = false;

private:
    RenderBuffers buffers;
    LevelConfig& config;
    float timer = 0;
    const float duration = 0.75f;
    const float fadeMeshRadius = 50.0f;
    u32 segments = 64;
    std::vector<float> mesh;
    std::vector<float> fadeMesh;
    float innerRadius = 100;
    float outerRadius = 400;
    float currentRadius = 0;
    Color outerColor = {0.0f, 0.0f, 0.0f, 1.0f};
    Color innerColor = {0.0f, 0.0f, 0.0f, 1.0f};
    Vector2 focus;
};


#endif //PLATFORMER_TRANSITIONEFFECT_H
