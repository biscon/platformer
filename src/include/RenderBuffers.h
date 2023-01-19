//
// Created by bison on 05-01-23.
//

#ifndef PLATFORMER_RENDERBUFFERS_H
#define PLATFORMER_RENDERBUFFERS_H

#include "../renderer/RenderCmdBuffer.h"

struct RenderBuffers {
    Renderer::RenderCmdBuffer& background;
    Renderer::RenderCmdBuffer& lit;
    Renderer::RenderCmdBuffer& unlit;
    Renderer::RenderCmdBuffer& light;
};

#endif //PLATFORMER_RENDERBUFFERS_H
