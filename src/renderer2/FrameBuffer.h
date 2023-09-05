//
// Created by bison on 05-09-23.
//

#ifndef PLATFORMER_FRAMEBUFFER_H
#define PLATFORMER_FRAMEBUFFER_H

#include "defs.h"
#include "Texture.h"
#include <memory>

namespace Renderer {

    class FrameBuffer {
    public:
        explicit FrameBuffer(TextureFormatInternal internalFormat, TextureFormatData format, i32 width, i32 height);
        void bind() const;
        static void unbind();
    private:
        u32 id = 0;
        std::unique_ptr<Texture> texture;
    };

} // Renderer

#endif //PLATFORMER_FRAMEBUFFER_H
