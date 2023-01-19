//
// Created by bison on 31-12-22.
//

#ifndef PLATFORMER_PARALLAXSCROLLER_H
#define PLATFORMER_PARALLAXSCROLLER_H

#include <defs.h>
#include <vector>
#include "../renderer/Types.h"
#include "../renderer/TextureAtlas.h"
#include "../renderer/RenderCmdBuffer.h"

using namespace Renderer;

struct ScrollLayer {
    float width, height;
    float posY;
    float offsetX;
    float speedFac;
    u32 atlasId;
    u32 quadIndexes[3];
    bool foreground;
};


class ParallaxScroller {
public:
    explicit ParallaxScroller(IRenderDevice& renderDevice);
    void createLayerFromPNG(const std::string& filename, float width, float height, float posY, float speed, bool foreground);
    void uploadTextures();
    void update(float targetX, float targetY);
    void render(RenderCmdBuffer &buffer, bool foreground);
    void reset();

private:
    IRenderDevice& renderDevice;

    std::vector<ScrollLayer> layers;
    std::vector<AtlasQuad> quads;
    std::shared_ptr<TextureAtlas> atlas;
    float directionX = 1;
    float scrollOffsetX = 0;
    float scrollOffsetY = 0;
};


#endif //PLATFORMER_PARALLAXSCROLLER_H
