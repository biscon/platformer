//
// Created by bison on 19-02-23.
//

#include "AnimationManager.h"

AnimationManager::AnimationManager(IRenderDevice &renderDevice) : renderDevice(renderDevice) {
    clear();
}

void AnimationManager::clear() {
    atlas = std::make_shared<TextureAtlas>(renderDevice, 2048, 2048, PixelFormat::RGBA);
    animations.clear();
}

void AnimationManager::upload() {
    if(atlas->getNoImages() > 0) {
        atlas->packAndUpload(renderDevice);
    }
}

void AnimationManager::rebuildAtlas() {
    atlas = std::make_shared<TextureAtlas>(renderDevice, 2048, 2048, PixelFormat::RGBA);
    for(auto& entry : animations) {
        auto info = entry.second;
        info.animation = Animation::createFromPNG(info.filename, info.frameWidth, info.frameHeight, info.originX, info.originY, info.fps, atlas,
                                             nullptr);
    }
}

void AnimationManager::add(const AnimationInfo& info) {
    animations[info.name] = info;
    animations[info.name].animation = Animation::createFromPNG(info.filename, info.frameWidth, info.frameHeight, info.originX, info.originY, info.fps, atlas,
                                              nullptr);

}

void AnimationManager::save(json& e) {
    for(auto& entry : animations) {
        auto &info = entry.second;
        json jsonInfo;
        jsonInfo["name"] = info.name;
        jsonInfo["filename"] = info.filename;
        jsonInfo["frameWidth"] = info.frameWidth;
        jsonInfo["frameHeight"] = info.frameHeight;
        jsonInfo["originX"] = info.originX;
        jsonInfo["originY"] = info.originY;
        jsonInfo["fps"] = info.fps;
        e.push_back(jsonInfo);
    }
}

void AnimationManager::load(const json &e) {
    for(auto& jsonInfo : e) {
        AnimationInfo info;
        info.name = jsonInfo["name"];
        info.filename = jsonInfo["filename"];
        info.frameWidth = jsonInfo["frameWidth"];
        info.frameHeight = jsonInfo["frameHeight"];
        info.originX = jsonInfo["originX"];
        info.originY = jsonInfo["originY"];
        info.fps = jsonInfo["fps"];
        add(info);
    }
}
