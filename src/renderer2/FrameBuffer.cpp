//
// Created by bison on 05-09-23.
//

#include <stdexcept>
#include "FrameBuffer.h"

extern "C" {
#include <glad.h>
}

namespace Renderer {
    FrameBuffer::FrameBuffer(TextureFormatInternal internalFormat, TextureFormatData format, i32 width, i32 height) {
        glGenFramebuffers(1, &id);
        bind();
        texture = std::make_unique<Texture>();
        texture->upload(width, height, nullptr, internalFormat, format);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO could not be completed!!");
        }
        texture->unbind();
        unbind();
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Renderer