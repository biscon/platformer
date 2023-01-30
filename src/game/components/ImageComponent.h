//
// Created by bison on 26-12-22.
//

#ifndef PLATFORMER_IMAGECOMPONENT_H
#define PLATFORMER_IMAGECOMPONENT_H

#include "../../renderer/TextureAtlas.h"
#include "../JsonUtil.h"

using namespace Renderer;

class ImageComponent {
public:
    ImageComponent(const ImageComponent& old);
    ImageComponent(const std::string& filename, IRenderDevice &renderDevice);
    std::string filename;
    float alpha = 1.0f;
    float width = 0;
    float height = 0;
    u32 textureId = 0;
    bool lightMap = false;
    FloatRect uvCoords;

    void save(json& e);
    void load(const json& e);
};


#endif //PLATFORMER_IMAGECOMPONENT_H
