//
// Created by bison on 19-02-23.
//

#ifndef PLATFORMER_ANIMATIONMANAGER_H
#define PLATFORMER_ANIMATIONMANAGER_H

#include <defs.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "../renderer/Types.h"
#include "../renderer/TextureAtlas.h"
#include "../renderer/Animation.h"
#include "JsonUtil.h"

using json = nlohmann::json;

using namespace Renderer;

struct AnimationInfo {
    std::string name;
    std::string filename;
    u32 frameWidth = 0;
    u32 frameHeight = 0;
    u32 originX = 0;
    u32 originY = 0;
    u16 fps = 0;
    std::shared_ptr<Animation> animation;
};

class AnimationManager {
public:
    AnimationManager(IRenderDevice &renderDevice);
    void clear();
    void upload();
    void rebuildAtlas();
    void add(const AnimationInfo& info);

    std::unordered_map<std::string, AnimationInfo>& getAnimations() {
        return animations;
    }

    void save(json& e);
    void load(const json& e);

private:
    IRenderDevice& renderDevice;
    std::unordered_map<std::string, AnimationInfo> animations;
    std::shared_ptr<TextureAtlas> atlas;
};


#endif //PLATFORMER_ANIMATIONMANAGER_H
