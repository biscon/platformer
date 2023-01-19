//
// Created by bison on 31-10-22.
//

#ifndef GAME_TYPES_H
#define GAME_TYPES_H


#include <defs.h>
#include <cmath>
#include <Vector2.h>

namespace Renderer {
    struct Color {
        float r, g, b, a;
    };

    const Renderer::Color BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
    const Renderer::Color WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
    const Renderer::Color RED = {1.0f, 0.0f, 0.0f, 1.0f};
    const Renderer::Color YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
    const Renderer::Color BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
    const Renderer::Color GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
    const Renderer::Color GREY = {0.5f, 0.5f, 0.5f, 1.0f};
    const Renderer::Color DARK_GREY = {0.3f, 0.3f, 0.3f, 1.0f};
    const Renderer::Color TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};



    struct Quad {
        Color                           color;
        float                           left;
        float                           top;
        float                           right;
        float                           bottom;

        void setBounds(FloatRect& bnds) {
            left = bnds.left;
            top = bnds.top;
            right = bnds.right;
            bottom = bnds.bottom;
        }
    };

    struct Vertex {
        Color                           color;
        float                           x;
        float                           y;
    };

    struct AtlasQuad {
        Color                           color;
        u32                             atlasId;
        float                           left;
        float                           top;
        float                           right;
        float                           bottom;
    };
}

#endif //GAME_TYPES_H
