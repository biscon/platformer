//
// Created by bison on 25-11-22.
//

#include "UpdateAnimationSystem.h"

void UpdateAnimationSystem::tick(World *world, float deltaTime) {
    world->each<SpriteComponent>([&](Entity* ent, ComponentHandle<SpriteComponent> sprite) {
        update(sprite.get(), deltaTime);
    });
}

void UpdateAnimationSystem::update(SpriteComponent &sprite, float deltaTime) {
    auto anim = sprite.getCurrentAnim();
    if(anim == nullptr) {
        return;
    }
    if(anim->ended || anim->paused)
        return;
    auto noFrames = anim->animation->noFrames;
    anim->timer -= deltaTime;
    if(anim->timer <= 0) {
        anim->timer = 1.0 / anim->animation->fps;
        if (!anim->reverse)
            anim->curFrame++;
        else
            anim->curFrame--;

        // if end of anim reached and not playing in reverse
        if (!anim->reverse && anim->curFrame > noFrames - 1) {
            if (anim->repeatType == RepeatType::Once) {
                anim->curFrame = noFrames - 1;
                if (anim->curFrame < 0)
                    anim->curFrame = 0;
                anim->ended = true;
            }
            if (anim->repeatType == RepeatType::Restart) {
                anim->curFrame = 0;
            }
            if (anim->repeatType == RepeatType::Reverse || anim->repeatType == RepeatType::ReverseOnce) {
                anim->curFrame = noFrames - 2;
                if (anim->curFrame < 0)
                    anim->curFrame = 0;
                anim->reverse = true;
            }
        } else if (anim->reverse && anim->curFrame < 0) // if anim is playing in reversed and has reached the beginning
        {
            if (anim->repeatType == RepeatType::Restart) {
                anim->curFrame = noFrames - 1;
                if (anim->curFrame < 0)
                    anim->curFrame = 0;
            }
            if (anim->repeatType == RepeatType::Reverse) {
                anim->curFrame = 1;
                if (anim->curFrame > noFrames - 1)
                    anim->curFrame = noFrames - 1;
                anim->reverse = false;
            }
            if (anim->repeatType == RepeatType::ReverseOnce) {
                anim->curFrame = 0;
                anim->ended = true;
            }
        }
        //SDL_Log("Animation %s current frame %d, reverse = %d", anim->name, anim->curFrame, anim->reverse);
    }
}
