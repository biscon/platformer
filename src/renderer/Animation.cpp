//
// Created by bison on 22-11-22.
//

#include <SDL_log.h>
#include "Animation.h"

namespace Renderer {
    AnimationRenderer::AnimationRenderer(const std::string &name,
                                         const std::shared_ptr<Animation> &animation,
                                         RepeatType repeatType) : name(name), animation(animation),
                                                                  repeatType(repeatType) {
        curFrame = 0;
        ended = false;
        paused = false;
        reverse = false;
        timer = 0;

    }

    std::shared_ptr<Animation>
    Animation::createFromPNG(const std::string &filename, u32 frame_width,
                             u32 frame_height, u32 ox, u32 oy, u16 fps,
                             const std::shared_ptr<TextureAtlas> &textureAtlas,
                             const FloatRect* insets) {
        auto anim = std::shared_ptr<Animation>(new Animation());
        anim->frameWidth = frame_width;
        anim->frameHeight = frame_height;
        anim->originX = ox;
        anim->originY = oy;
        anim->fps = fps;
        PixelBuffer sheet_pb(filename, false);
        anim->widthInFrames = sheet_pb.width / frame_width;
        anim->heightInFrames = sheet_pb.height / frame_height;
        anim->noFrames = anim->widthInFrames * anim->heightInFrames;
        anim->textureAtlas = textureAtlas;

        for(u32 y = 0; y < anim->heightInFrames; ++y) {
            for(u32 x = 0; x < anim->widthInFrames; ++x) {
                auto pb = std::make_unique<PixelBuffer>(anim->frameWidth+2, anim->frameHeight+2, PixelFormat::RGBA);
                pb->padding = true;
                pb->copyFrom(sheet_pb, UIntRect(x * anim->frameWidth, anim->frameHeight * y, anim->frameWidth, anim->frameHeight), UIntPos(1,1));

                auto size = pb->getTrimmedSize();
                UIntRect sizeI = UIntRect(0, 0, pb->width - 2, pb->height - 2);

                u32 frame_id = textureAtlas->addImage(std::move(pb));

                FrameInfo info;
                info.id = frame_id;
                info.combatBox = FloatRect(size.x, size.y, size.x + size.w, size.y + size.h);

                if(insets != nullptr) {
                    sizeI.x = (u32) ((float) sizeI.w * insets->left);
                    sizeI.w -= (u32) ((float) sizeI.w * insets->right) + sizeI.x;
                    sizeI.y = (u32) ((float) sizeI.h * insets->top);
                    sizeI.h -= (u32) ((float) sizeI.h * insets->bottom) + sizeI.y;
                    SDL_Log("Sprite anim frame insetted: %d,%d,%d,%d", sizeI.x, sizeI.y, sizeI.w, sizeI.h);
                    info.box = FloatRect(sizeI.x, sizeI.y, sizeI.x + sizeI.w, sizeI.y + sizeI.h);

                } else {
                    info.box = info.combatBox;
                }
                anim->frameInfos.emplace_back(info);

            }
        }

        return anim;
    }
}