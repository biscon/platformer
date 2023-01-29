//
// Created by bison on 31-12-22.
//

#include <SDL_log.h>
#include "ParallaxScroller.h"

ParallaxScroller::ParallaxScroller(IRenderDevice& renderDevice) : renderDevice(renderDevice) {
    reset();
}

void ParallaxScroller::createLayerFromPNG(const std::string &filename, float width, float height,
                                          float posY, float speed, bool foreground) {
    ScrollLayer layer{};
    layer.width = width;
    layer.height = height;
    layer.posY = posY;
    layer.speedFac = speed;
    layer.foreground = foreground;
    layer.atlasId = atlas->addImageFromPNG(filename, true);
    layer.filename = filename;

    AtlasQuad quad1 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = layer.atlasId,
            .left = -layer.width, .top = posY, .right = 0, .bottom = posY + layer.height};
    AtlasQuad quad2 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = layer.atlasId,
            .left = 0, .top = posY, .right = layer.width, .bottom = posY + layer.height};
    AtlasQuad quad3 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = layer.atlasId,
            .left = layer.width, .top = posY, .right = 2 * layer.width, .bottom = posY + layer.height};

    quads.push_back(quad1);
    layer.quadIndexes[0] = (u32) quads.size() - 1;
    quads.push_back(quad2);
    layer.quadIndexes[1] = (u32) quads.size() - 1;
    quads.push_back(quad3);
    layer.quadIndexes[2] = (u32) quads.size() - 1;
    layer.built = true;
    layers.push_back(layer);
}

void ParallaxScroller::rebuild() {
    quads.clear();
    directionX = 1;
    scrollOffsetX = 0;
    scrollOffsetY = 0;
    this->atlas = std::make_shared<TextureAtlas>(renderDevice, 1024, 1024, PixelFormat::RGBA);
    for(auto& layer : layers) {
        layer.atlasId = atlas->addImageFromPNG(layer.filename, true);
        AtlasQuad quad1 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = layer.atlasId,
                .left = -layer.width, .top = layer.posY, .right = 0, .bottom = layer.posY + layer.height};
        AtlasQuad quad2 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = layer.atlasId,
                .left = 0, .top = layer.posY, .right = layer.width, .bottom = layer.posY + layer.height};
        AtlasQuad quad3 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = layer.atlasId,
                .left = layer.width, .top = layer.posY, .right = 2 * layer.width, .bottom = layer.posY + layer.height};

        quads.push_back(quad1);
        layer.quadIndexes[0] = (u32) quads.size() - 1;
        quads.push_back(quad2);
        layer.quadIndexes[1] = (u32) quads.size() - 1;
        quads.push_back(quad3);
        layer.quadIndexes[2] = (u32) quads.size() - 1;
        layer.built = true;
    }
    uploadTextures();
}

void ParallaxScroller::uploadTextures() {
    atlas->packAndUpload(renderDevice);
}

void ParallaxScroller::update(float targetX, float targetY) {
    float scrollX = targetX - scrollOffsetX;
    float scrollY = targetY - scrollOffsetY;
    if(scrollX == 0 && scrollY == 0) {
        return;
    }
    directionX = glm_signf(scrollX);

    for(u8 i = 0; i < layers.size(); ++i) {
        ScrollLayer *layer = &layers[i];
        if(!layer->built) {
            continue;
        }
        double speed = layer->speedFac * scrollX;
        speed *= -1;

        layer->offsetX += speed;
        float truncX = layer->offsetX;

        //SDL_Log("layer offsetX = %.2f", layer->offsetX);

        AtlasQuad* quads[3] = {
                &this->quads[layer->quadIndexes[0]],
                &this->quads[layer->quadIndexes[1]],
                &this->quads[layer->quadIndexes[2]],
        };

        // left
        quads[0]->left = truncX - layer->width;
        quads[0]->right = truncX;
        // middle
        quads[1]->left = truncX;
        quads[1]->right = truncX + layer->width;
        // right
        quads[2]->left = truncX + layer->width;
        quads[2]->right = truncX + layer->width + layer->width;

        if(directionX == 1) {
            if(quads[2]->left <= layer->width) {
                quads[2]->left = truncX + layer->width;
                quads[2]->right = truncX + layer->width + layer->width;
                layer->offsetX = truncX + layer->width;
            }
        }
        if(directionX == -1) {
            if(quads[0]->right >= 0) {
                quads[0]->left = truncX - layer->width;
                quads[0]->right = truncX;
                layer->offsetX = truncX - layer->width;
            }
        }
    }
    scrollOffsetX += scrollX;
    scrollOffsetY += scrollY;
    //SDL_Log("ScrollOffset = %.2f, %.2f", scrollOffsetX, scrollOffsetY);
}

void ParallaxScroller::render(RenderCmdBuffer &buffer, bool foreground) {
    if(layers.empty()) {
        return;
    }
    mat4 transform = {};
    glm_mat4_identity(transform);
    glm_translate_y(transform, -scrollOffsetY);
    buffer.pushTransform(&transform);

    for(u8 i = 0; i < layers.size(); ++i) {
        ScrollLayer *layer = &layers[i];
        if(layer->foreground == foreground && layer->built) {
            for(auto index : layer->quadIndexes) {
                buffer.pushAtlasQuad(quads[index], atlas.get());
            }
        }
    }

    glm_mat4_identity(transform);
    buffer.pushTransform(&transform);
}

void ParallaxScroller::reset() {
    layers.clear();
    quads.clear();
    directionX = 1;
    scrollOffsetX = 0;
    scrollOffsetY = 0;
    this->atlas = std::make_shared<TextureAtlas>(renderDevice, 1024, 1024, PixelFormat::RGBA);
}

bool ParallaxScroller::moveLayerUp(u32 i) {
    if(i > 0) {
        iter_swap(layers.begin() + i, layers.begin() + i - 1);
        return true;
    }
    return false;
}

bool ParallaxScroller::moveLayerDown(u32 i) {
    if(i < layers.size()-1) {
        iter_swap(layers.begin() + i, layers.begin() + i + 1);
        return true;
    }
    return false;
}

bool ParallaxScroller::addLayer(u32 i) {
    if((i >= 0 && i < layers.size()) || layers.empty()) {
        ScrollLayer layer{0};
        layers.insert(layers.begin() + i, layer);
        return true;
    }
    return false;
}

bool ParallaxScroller::deleteLayer(u32 i) {
    if(i >= 0 && i < layers.size()) {
        layers.erase(layers.begin() + i);
        return true;
    }
    return false;
}
