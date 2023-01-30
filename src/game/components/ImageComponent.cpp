//
// Created by bison on 26-12-22.
//

#include "ImageComponent.h"

ImageComponent::ImageComponent(const std::string &filename,
                               IRenderDevice &renderDevice): filename(filename) {
    auto pb = std::make_unique<PixelBuffer>(filename, false);
    width = pb->width;
    height = pb->height;
    //pb->upscaleNPOT();
    uvCoords.left = 0;
    uvCoords.top = 0;
    uvCoords.right = width / (float) pb->width;
    uvCoords.bottom = height / (float) pb->height;
    textureId = renderDevice.uploadTexture(pb.get(), false);
}

ImageComponent::ImageComponent(const ImageComponent &old) {
    width = old.width;
    height = old.height;
    alpha = old.alpha;
    textureId = old.textureId;
    filename = old.filename;
    uvCoords = old.uvCoords;
}

void ImageComponent::save(json &e) {
    json t;
    t["filename"] = filename;
    t["alpha"] = alpha;
    t["width"] = width;
    t["height"] = height;
    t["lightMap"] = lightMap;
    //t["uvCoords"] = {uvCoords.left, uvCoords.top, uvCoords.right, uvCoords.bottom};
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
    //auto coords = e["uvCoords"];
    //uvCoords = FloatRect(coords[0], coords[1], coords[2], coords[3]);
    filename = e["filename"];
}
