//
// Created by bison on 05-09-23.
//

#ifndef PLATFORMER_TEXTURE_H
#define PLATFORMER_TEXTURE_H

#include <defs.h>

namespace Renderer {

    enum class TextureFormatInternal {
        R8,
        RGB8,
        RGBA8,
    };

    enum class TextureFormatData {
        RED,
        RGB,
        RGBA,
    };

    enum class TextureFiltering {
        NEAREST,
        LINEAR,
    };

    class Texture {
    public:
        Texture();
        ~Texture();
        void bind() const;
        static void unbind();
        void upload(i32 width, i32 height, u8* data, TextureFormatInternal internalFormat, TextureFormatData format) const;
        void setFiltering(TextureFiltering filtering) const;
    private:
        u32 id = 0;
        friend class FrameBuffer;
    };

} // Renderer

#endif //PLATFORMER_TEXTURE_H
