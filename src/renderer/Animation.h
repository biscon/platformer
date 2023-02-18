//
// Created by bison on 22-11-22.
//

#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H

#include <string>
#include <memory>
#include "Types.h"
#include "TextureAtlas.h"

namespace Renderer {
    enum class RepeatType {
        Once,
        Restart,
        Reverse,
        ReverseOnce
    };

    struct FrameInfo {
        u32 id{};
        FloatRect box;
        FloatRect combatBox;
    };

    struct Animation {
    public:
        Vector2 origin;
        u16 fps{};
        u32 frameWidth{};
        u32 frameHeight{};
        u32 widthInFrames{};
        u32 heightInFrames{};
        u32 noFrames{};

        std::shared_ptr<TextureAtlas> textureAtlas;
        std::vector<FrameInfo> frameInfos;
        std::string filename;

        static std::shared_ptr<Animation> createFromPNG(const std::string& filename,
                                                        u32 frame_width, u32 frame_height, u32 ox, u32 oy, u16 fps,
                                                        const std::shared_ptr<TextureAtlas>& textureAtlas,
                                                        const FloatRect* insets);

    private:
        Animation() = default;
    };

    struct AnimationRenderer {
        AnimationRenderer(const std::string &name, const std::shared_ptr<Animation> &animation,
                          RepeatType repeatType);

        std::string name;
        std::shared_ptr<Animation> animation;
        RepeatType repeatType;
        bool reverse;
        bool paused;
        bool ended;
        double timer;
        i32 curFrame;
    };
}

#endif //GAME_ANIMATION_H
