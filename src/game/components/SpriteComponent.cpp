//
// Created by bison on 10-11-22.
//

#include <SDL_log.h>
#include "SpriteComponent.h"

namespace Renderer {

    SpriteComponent::SpriteComponent(const std::string &name) : name(name) {
        curAnimIndex = -1;
        alpha = 1.0f;
        vertFlip = false;
        horizFlip = false;
    }

    void SpriteComponent::createAnimation(const std::string &name, RepeatType repeatType,
                                 std::shared_ptr<Animation> &anim) {
        AnimationRenderer renderer(name, anim, repeatType);
        animations.emplace_back(renderer);
    }

    void SpriteComponent::pause() {
        if(curAnimIndex >= 0 && curAnimIndex < (i32) animations.size()) {
            animations[curAnimIndex].paused = true;
        }
    }

    void SpriteComponent::resume() {
        if(curAnimIndex >= 0 && curAnimIndex < (i32) animations.size()) {
            animations[curAnimIndex].paused = false;
        }
    }

    bool SpriteComponent::hasAnimEnded() {
        if(curAnimIndex >= 0 && curAnimIndex < (i32) animations.size()) {
            return animations[curAnimIndex].ended;
        }
        return true;
    }

    void SpriteComponent::setAnim(const std::string &name) {
        i32 i = findAnimIndexByName(name);
        if(i != -1) {
            curAnimIndex = i;
            animations[i].curFrame = 0;
            animations[i].ended = false;
            animations[i].reverse = false;
        }
    }

    void SpriteComponent::setAnimIfNotCur(const std::string &name) {
        i32 i = findAnimIndexByName(name);
        if(i != -1 && curAnimIndex != i) {
            curAnimIndex = i;
            animations[i].curFrame = 0;
            animations[i].ended = false;
            animations[i].reverse = false;
        }
    }

    void SpriteComponent::getOrigin(float *ox, float *oy, float scale) {
        if(curAnimIndex == -1) {
            return;
        }
        const auto& anim = animations[curAnimIndex];
        *ox = anim.animation->originX;
        *oy = anim.animation->originY;
        if(horizFlip) {
            *ox = anim.animation->frameWidth - anim.animation->originX;
        }
        if(vertFlip) {
            *oy = anim.animation->frameHeight - anim.animation->originY;
        }
        *ox *= scale;
        *oy *= scale;
    }

    const FrameInfo *SpriteComponent::getFrameInfo() {
        if(curAnimIndex == -1) {
            return nullptr;
        }
        const auto& anim = animations[curAnimIndex];
        return &anim.animation->frameInfos[anim.curFrame];
    }

    void SpriteComponent::getBoundingBox(FloatRect *rect) {
        auto frame = getFrameInfo();
        auto anim = getCurrentAnim();
        if(frame == nullptr || anim == nullptr) {
            return;
        }
        *rect = frame->box;

        if(horizFlip) {
            rect->left = anim->animation->frameWidth - frame->box.width() - frame->box.left;
            rect->right = rect->left + frame->box.width();
        }
        if(vertFlip) {
            //rect->top = anim->animation->frameHeight - frame->box.height() - frame->box.top;
            //rect->bottom = rect->top + frame->box.height();
        }
    }

    void SpriteComponent::getCombatBox(FloatRect *rect) {
        auto info = getFrameInfo();
        if(info == nullptr) {
            return;
        }
        *rect = info->combatBox;
    }

    const AnimationRenderer *SpriteComponent::findAnimByName(const std::string &name) {
        for(auto& anim : animations) {
            if(anim.name == name)
                return &anim;
        }
        return nullptr;
    }

    i32 SpriteComponent::findAnimIndexByName(const std::string &name) {
        i32 index = -1;
        for(auto& anim : animations) {
            index++;
            if(anim.name == name)
                return index;
        }
        return -1;
    }

    AnimationRenderer *SpriteComponent::getCurrentAnim() {
        if(curAnimIndex > -1) {
            auto& anim = animations[curAnimIndex];
            return &anim;
        }
        return nullptr;
    }
}