//
// Created by bison on 26-12-22.
//

#include "ImageComponent.h"

ImageComponent::ImageComponent(const std::string &filename,
                               const std::shared_ptr<TextureAtlas> &textureAtlas): filename(filename), textureAtlas(textureAtlas) {
    auto pb = std::make_unique<PixelBuffer>(filename, false);
    width = pb->width;
    height = pb->height;
    atlasId = textureAtlas->addImage(std::move(pb));
}

ImageComponent::ImageComponent(const ImageComponent &old) {
    textureAtlas = old.textureAtlas;
    width = old.width;
    height = old.height;
    alpha = old.alpha;
    atlasId = old.atlasId;
    filename = old.filename;
}

void ImageComponent::save(json &e) {
    json t;
    t["filename"] = filename;
    t["alpha"] = alpha;
    t["width"] = width;
    t["height"] = height;
    t["lightMap"] = lightMap;
    e["image"] = t;
}

void ImageComponent::load(const json& e) {
    alpha = e["alpha"];
    width = e["width"];
    height = e["height"];
    if(e.contains("lightMap"))
        lightMap = e["lightMap"];
    else
        lightMap = false;
    filename = e["filename"];
}
