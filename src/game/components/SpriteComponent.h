//
// Created by bison on 10-11-22.
//

#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include <string>
#include <memory>
#include <Vector2.h>
#include "../../renderer/Types.h"
#include "../../renderer/TextureAtlas.h"
#include "../../renderer/Animation.h"
#include "../JsonUtil.h"
#include "../AnimationManager.h"

using json = nlohmann::json;

namespace Renderer {
    class SpriteComponent {
    public:
        SpriteComponent();
        SpriteComponent(const json& e, AnimationManager& animManager);
        void save(json& e);

        void createAnimation(const std::string& name, RepeatType repeatType, std::shared_ptr<Animation>& anim);
        void pause();
        void resume();
        bool hasAnimEnded();
        void setAnim(const std::string& name);
        void setAnimIndex(i32 i);
        void setAnimIfNotCur(const std::string& name);
        void getOrigin(float* ox, float *oy, float scale);
        const FrameInfo* getFrameInfo();
        void getBoundingBox(FloatRect *rect);
        void getCombatBox(FloatRect *rect);
        AnimationRenderer* getCurrentAnim();

        std::vector<AnimationRenderer>& getAnimations() {
            return animations;
        }


        bool vertFlip;
        bool horizFlip;
        float alpha;

    private:
        i32 curAnimIndex;
        std::vector<AnimationRenderer> animations;

        const AnimationRenderer* findAnimByName(const std::string& name);
        i32 findAnimIndexByName(const std::string &name);
    };
}

#endif //GAME_SPRITE_H
