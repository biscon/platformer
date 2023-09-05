//
// Created by bison on 05-09-23.
//

#include <SDL_log.h>
#include <stdexcept>
#include "Texture.h"

extern "C" {
#include <glad.h>
}

namespace Renderer {
    static i32 getGLInternalFormat(TextureFormatInternal format) {
        switch(format) {
            case TextureFormatInternal::R8: return GL_R8;
            case TextureFormatInternal::RGB8: return GL_RGB8;
            case TextureFormatInternal::RGBA8: return GL_RGBA8;
            default:
                throw std::runtime_error("Unknown texture internal format");
        }
    }

    static GLenum getGLFormat(TextureFormatData format) {
        switch(format) {
            case TextureFormatData::RED: return GL_RED;
            case TextureFormatData::RGB: return GL_RGB;
            case TextureFormatData::RGBA: return GL_RGBA;
            default:
                throw std::runtime_error("Unknown texture data format");
        }
    }


    Texture::Texture() {
        glGenTextures(1, &id);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &id);
    }

    void Texture::bind() const {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void Texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::upload(i32 width, i32 height, u8 *data, TextureFormatInternal internalFormat, TextureFormatData format) const {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, getGLInternalFormat(internalFormat),
                     width, height, 0, getGLFormat(format), GL_UNSIGNED_BYTE, (const void*) data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void Texture::setFiltering(TextureFiltering filtering) const {
        bind();
        switch(filtering) {
            case TextureFiltering::NEAREST:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFiltering::LINEAR:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }
    }

} // Renderer