//
// Created by bison on 27-10-22.
//

#include "OGLRenderDevice.h"
#include <cglm/cam.h>
#include <SDL_log.h>
#include <stdexcept>

namespace Renderer {

    namespace {
        const float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 1.0f
        };
    }

    OGLRenderDevice::OGLRenderDevice(i32 width, i32 height) {
        screenWidth = width;
        screenHeight = height;
        currentViewport.x = 0;
        currentViewport.y = 0,
        currentViewport.w = width;
        currentViewport.h = height;

        loadShaderPrograms();

        // setup framebuffer for light rendering
        glGenFramebuffers(1, &lightFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
        glGenTextures(1, &lightTextureId);
        glBindTexture(GL_TEXTURE_2D, lightTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTextureId, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO could not be completed!!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // setup framebuffer for the background, no lighting
        glGenFramebuffers(1, &bgFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, bgFBO);
        glGenTextures(1, &bgTextureId);
        glBindTexture(GL_TEXTURE_2D, bgTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bgTextureId, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO could not be completed!!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // setup framebuffer for stuff not affected by light
        glGenFramebuffers(1, &unlitFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, unlitFBO);
        glGenTextures(1, &unlitTextureId);
        glBindTexture(GL_TEXTURE_2D, unlitTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, unlitTextureId, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO could not be completed!!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Setup framebuffer for rendering of stuff affected by light
        glGenFramebuffers(1, &litFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, litFBO);

        glGenTextures(1, &litTextureId);
        glBindTexture(GL_TEXTURE_2D, litTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, litTextureId, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO could not be completed!!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // setup offscreen rendering VAO
        glGenVertexArrays(1, &screenQuadVAO);
        glGenBuffers(1, &screenQuadVBO);
        glBindVertexArray(screenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), &screenQuadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // generate and bind VAO
        glGenVertexArrays(1, &texQuadVAO);
        glBindVertexArray(texQuadVAO);

        // generate and bind buffer
        glGenBuffers(1, &texQuadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texQuadVBO);

        GLsizei bytes_vertex = FLOATS_PER_VERTEX * sizeof(float);
        // setup vertex attributes
        // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, bytes_vertex, (void*) nullptr);
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, bytes_vertex, (void *) (2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // tex coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, bytes_vertex, (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    OGLRenderDevice::~OGLRenderDevice() {
        glDeleteBuffers(1, &texQuadVBO);
        glDeleteVertexArrays(1, &texQuadVAO);

        glDeleteBuffers(1, &screenQuadVBO);
        glDeleteVertexArrays(1, &screenQuadVAO);

        glDeleteTextures(1, &lightTextureId);
        glDeleteFramebuffers(1, &lightFBO);

        glDeleteTextures(1, &litTextureId);
        glDeleteFramebuffers(1, &litFBO);

        glDeleteTextures(1, &unlitTextureId);
        glDeleteFramebuffers(1, &unlitFBO);

        glDeleteTextures(1, &bgTextureId);
        glDeleteFramebuffers(1, &bgFBO);

        glDeleteProgram(texQuadProgramID);
    }


    void OGLRenderDevice::renderCmdBuffer(RenderCmdBuffer &buf) {
        u8 *cur_ptr = buf.commands;
        while(cur_ptr < buf.cmdOffset) {

            //SDL_Log("Iteration %d", iteration);

            Command *command = (Command*) cur_ptr;
            switch(command->type)
            {
                case CommandType::Clear: {
                    ClearCommand *cmd = (ClearCommand*) cur_ptr;
                    //SDL_Log("Rendering clear command");
                    cur_ptr += sizeof(ClearCommand);
                    glClearColor(cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a);
                    glClear(GL_COLOR_BUFFER_BIT);
                    break;
                }
                case CommandType::EnableMatte: {
                    EnableMatteCommand *cmd = (EnableMatteCommand*) cur_ptr;
                    cur_ptr += sizeof(EnableMatteCommand);
                    glUniform1i(uniformMatte, 1);
                    float col[3] = {cmd->color.r, cmd->color.g, cmd->color.b};
                    glUniform3fv(uniformMatteColor, 1, col);
                    break;
                }
                case CommandType::DisableMatte: {
                    //DisableMatteCommand *cmd = (DisableMatteCommand*) cur_ptr;
                    cur_ptr += sizeof(DisableMatteCommand);
                    glUniform1i(uniformMatte, 0);
                    break;
                }
                case CommandType::Transform: {
                    TransformCommand *cmd = (TransformCommand*) cur_ptr;
                    cur_ptr += sizeof(TransformCommand);
                    glUniformMatrix4fv(uniformModelLoc, 1, GL_FALSE, (float*) cmd->matrix);
                    break;
                }
                case CommandType::Quad: {
                    QuadCommand *cmd = (QuadCommand*) cur_ptr;
                    //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                    cur_ptr += sizeof(QuadCommand);
                    glUniform1i(uniformIsFontLoc, 0);
                    glUniform1i(uniformUseTextureLoc, 0);
                    if(cmd->premultipliedAlpha) {
                        enablePremultipliedAlpha();
                    } else {
                        enableRegularAlpha();
                    }
                    glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                    break;
                }
                case CommandType::LineLoop: {
                    VerticesCommand *cmd = (VerticesCommand*) cur_ptr;
                    //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                    cur_ptr += sizeof(VerticesCommand);
                    glUniform1i(uniformIsFontLoc, 0);
                    glUniform1i(uniformUseTextureLoc, 0);
                    enableRegularAlpha();
                    glDrawArrays(GL_LINE_LOOP, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                    enablePremultipliedAlpha();
                    break;
                }
                case CommandType::TriangleStripMesh: {
                    VerticesCommand *cmd = (VerticesCommand*) cur_ptr;
                    //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                    cur_ptr += sizeof(VerticesCommand);
                    glUniform1i(uniformIsFontLoc, 0);
                    glUniform1i(uniformUseTextureLoc, 0);
                    enableRegularAlpha();
                    glDrawArrays(GL_TRIANGLE_STRIP, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                    enablePremultipliedAlpha();
                    break;
                }
                case CommandType::Triangles: {
                    VerticesCommand *cmd = (VerticesCommand*) cur_ptr;
                    //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                    cur_ptr += sizeof(VerticesCommand);
                    glUniform1i(uniformIsFontLoc, 0);
                    glUniform1i(uniformUseTextureLoc, 0);
                    enableRegularAlpha();
                    glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                    enablePremultipliedAlpha();
                    break;
                }
                case CommandType::TexQuad: {
                    QuadCommand *cmd = (QuadCommand*) cur_ptr;
                    //SDL_Log("Rendering tex quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                    cur_ptr += sizeof(QuadCommand);
                    glUniform1i(uniformIsFontLoc, 0);
                    glUniform1i(uniformUseTextureLoc, 1);
                    glBindTexture(GL_TEXTURE_2D, cmd->texId);
                    if(cmd->premultipliedAlpha) {
                        enablePremultipliedAlpha();
                    } else {
                        enableRegularAlpha();
                    }
                    glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                    break;
                }
                case CommandType::TexQuadAtlas: {
                    QuadCommand *cmd = (QuadCommand*) cur_ptr;
                    cur_ptr += sizeof(QuadCommand);
                    i32 is_font = 0;
                    if(cmd->atlas->format == PixelFormat::GREYSCALE) {
                        is_font = 1;
                    }
                    //SDL_Log("Rendering atlas quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld is_font = %d", cmd->offset, cmd->vertexCount, cmd->vertexOffset, is_font);
                    glUniform1i(uniformIsFontLoc, is_font);
                    glUniform1i(uniformUseTextureLoc, 1);
                    glBindTexture(GL_TEXTURE_2D, cmd->texId);

                    if(cmd->premultipliedAlpha) {
                        enablePremultipliedAlpha();
                    } else {
                        enableRegularAlpha();
                    }

                    glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    void OGLRenderDevice::renderLightBuffer(RenderCmdBuffer &buf) {
        // set render view port
        glViewport(0, 0, screenWidth, screenHeight);
        // First pass, change render target to offscreen framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);

        glUseProgram(texQuadProgramID);

        // bind vao
        glBindVertexArray(texQuadVAO);
        // upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, texQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, (buf.verticesOffset - buf.vertices), buf.vertices, GL_DYNAMIC_DRAW);

        // setup matrices, right now we only really need the projection matrix
        mat4 model = {};
        mat4 view = {};
        mat4 proj = {};

        glm_mat4_identity(model);
        glm_mat4_identity(view);
        glm_ortho(0, screenWidth, screenHeight, 0, -100.0f, 100.0f, proj);

        // set uniforms
        glUniformMatrix4fv(uniformModelLoc, 1, GL_FALSE, (float*) model);
        glUniformMatrix4fv(uniformViewLoc, 1, GL_FALSE, (float*) view);
        glUniformMatrix4fv(uniformProjLoc, 1, GL_FALSE, (float*) proj);

        // bind diffuse texture to unit 0
        glUniform1i(uniformTextureLoc, 0);
        glUniform1i(uniformUseLut, 0);

        // disable depth testing since we're using blending and painters algorithm
        glDisable(GL_DEPTH_TEST);

        // enable alpha blending
        glEnable(GL_BLEND);
        enableAdditiveAlpha();

        glEnable( GL_LINE_SMOOTH );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

        renderCmdBuffer(buf);

        // set actual viewport based on current window size
        /*
        glViewport(currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);

        // second pass, render offscreen texture as a quad filling up the entire screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(screenQuadProgramID);
        // bind sampler to texture unit 0
        glUniform1i(uniformScreenTexture, 0);
        glUniform1i(uniformUseCrt, useCrt ? 1 : 0);


        glBindVertexArray(screenQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, lightTextureId);
        glDrawArrays(GL_TRIANGLES, 0, 6);
         */
    }

    void OGLRenderDevice::renderBuffer(RenderCmdBuffer &buf, GLuint fbo, bool useLut) {
        // set render view port
        glViewport(0, 0, screenWidth, screenHeight);
        // First pass, change render target to offscreen framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glUseProgram(texQuadProgramID);

        // bind color lut texture
        if (useLut && lutActive) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, lut1TextureId);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, lut2TextureId);
            glActiveTexture(GL_TEXTURE0);
        }


        // bind vao
        glBindVertexArray(texQuadVAO);
        // upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, texQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, (buf.verticesOffset - buf.vertices), buf.vertices, GL_DYNAMIC_DRAW);

        // setup matrices, right now we only really need the projection matrix
        mat4 model = {};
        mat4 view = {};
        mat4 proj = {};

        glm_mat4_identity(model);
        glm_mat4_identity(view);
        glm_ortho(0, screenWidth, screenHeight, 0, -100.0f, 100.0f, proj);

        // set uniforms
        glUniformMatrix4fv(uniformModelLoc, 1, GL_FALSE, (float*) model);
        glUniformMatrix4fv(uniformViewLoc, 1, GL_FALSE, (float*) view);
        glUniformMatrix4fv(uniformProjLoc, 1, GL_FALSE, (float*) proj);

        // bind diffuse texture to unit 0
        glUniform1i(uniformTextureLoc, 0);

        if(useLut && lutActive) {
            // bind Lut1 to texture unit 1
            glUniform1i(uniformLut1Texture, 1);
            // bind Lut2 to texture unit 2
            glUniform1i(uniformLut2Texture, 2);

            glUniform1i(uniformUseLut, 1);
            glUniform1f(uniformLutMix, lutMix);
        } else {
            glUniform1i(uniformUseLut, 0);
        }

        // disable depth testing since we're using blending and painters algorithm
        glDisable(GL_DEPTH_TEST);

        // enable alpha blending
        glEnable(GL_BLEND);
        enablePremultipliedAlpha();

        glEnable( GL_LINE_SMOOTH );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

        renderCmdBuffer(buf);
    }

    void OGLRenderDevice::renderUnlitBuffer(RenderCmdBuffer &buf) {
        renderBuffer(buf, unlitFBO, false);
    }

    void OGLRenderDevice::renderLitBuffer(RenderCmdBuffer &buf) {
        renderBuffer(buf, litFBO, false);
    }

    void OGLRenderDevice::renderBackgroundBuffer(RenderCmdBuffer &buf) {
        renderBuffer(buf, bgFBO, true);
    }

    void OGLRenderDevice::renderScreen() {
        // set actual viewport based on current window size
        glViewport(currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);

        // second pass, render offscreen texture as a quad filling up the entire screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(screenQuadProgramID);
        // bind lit texture unit 0
        glUniform1i(uniformScreenTexture, 0);
        // bind light texture to unit 1
        glUniform1i(uniformLightTexture, 1);
        glUniform1i(uniformUseCrt, useCrt ? 1 : 0);

        glBindVertexArray(screenQuadVAO);
        glDisable(GL_DEPTH_TEST);

        // render background
        glEnable(GL_BLEND);
        enableRegularAlpha();
        glUniform1i(uniformUseLight, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgTextureId);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Render lit geometry, blending is done in the shader
        //glDisable(GL_BLEND);
        enablePremultipliedAlpha();
        glUniform1i(uniformUseLight, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lightTextureId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, litTextureId);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // render unlit geometry in front
        glEnable(GL_BLEND);
        enablePremultipliedAlpha();
        glUniform1i(uniformUseLight, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, unlitTextureId);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    u32 OGLRenderDevice::uploadPNGTexture(const std::string& filename, bool filtering) {
        auto pb = std::make_unique<PixelBuffer>(filename.c_str(), false);
        return uploadTexture(pb.get(), filtering);
    }

    u32 OGLRenderDevice::uploadTexture(PixelBuffer *pb, bool filtering)
    {
        pb->preMultiplyAlpha();
        u32 tex = 0;
        glGenTextures(1, &tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        // upload pixel data to gpu mem
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pb->width, pb->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pb->pixels);

        // set wrap clamp
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // nearest neighbour filtering
        if(!filtering) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            // anisotropic filtering
            /*
            float aniso = 16.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
            */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }


        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            SDL_Log("UploadTextureOGL OpenGL error: %d", err);
            throw std::runtime_error("UploadTextureOGL OpenGL error: %d");
        }

        return tex;
    }

    u32 OGLRenderDevice::uploadTextureGreyscale(PixelBuffer *pb, bool filtering)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        u32 tex = 0;
        glGenTextures(1, &tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        // upload pixel data to gpu mem
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pb->width, pb->height, 0, GL_RED, GL_UNSIGNED_BYTE, pb->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // nearest neighbour filtering
        if(!filtering) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            SDL_Log("UploadTextureOGL OpenGL error: %d", err);
            throw std::runtime_error("UploadTextureOGL OpenGL error: %d");
        }
        return tex;
    }


    void OGLRenderDevice::deleteTexture(u32 *tex) {
        glDeleteTextures(1, (GLuint*) &tex);
    }

    /**
     * Finds optimal viewport size given windows size while preservering aspect ratios
     * viewport will be either letter - or pillarboxed when the aspect ratio of the
     * window differs from the games "native" aspect ratio
     * @param newWidth
     * @param newHeight
     */
    void OGLRenderDevice::viewportSizeChanged(i32 newWidth, i32 newHeight) {
        u32 realWidth = (u32) newWidth;
        u32 realHeight = (u32) newHeight;

        float want_aspect;
        float real_aspect;
        float scale;

        want_aspect = (float) screenWidth / screenHeight;
        real_aspect = (float) newWidth / newHeight;


        if (SDL_fabs(want_aspect-real_aspect) < 0.0001) {
            /* The aspect ratios are the same, just scale appropriately */
            currentViewport.x = 0;
            currentViewport.y = 0;
            currentViewport.w = realWidth;
            currentViewport.h = realHeight;
            glViewport(currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);
            //SDL_Log("Same aspect ratio");
        } else if (want_aspect > real_aspect) {
            /* We want a wider aspect ratio than is available - letterbox it */
            scale = (float) newWidth / screenWidth;
            currentViewport.x = 0;
            currentViewport.w = newWidth;
            currentViewport.h = (int)SDL_ceil(screenHeight * scale);
            currentViewport.y = (newHeight - currentViewport.h) / 2;
            glViewport(currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);
            //SDL_Log("letterbox");
        } else {
            /* We want a narrower aspect ratio than is available - use side-bars */
            scale = (float) newHeight / screenHeight;
            currentViewport.y = 0;
            currentViewport.h = newHeight;
            currentViewport.w = (int)SDL_ceil(screenWidth * scale);
            currentViewport.x = (newWidth - currentViewport.w) / 2;
            glViewport(currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);
            //SDL_Log("sidebars");
        }
        //SDL_Log("Viewport changed to %d,%d, %d,%d", currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);
    }

    void OGLRenderDevice::setLut1Texture(u32 tex) {
        lut1TextureId = tex;
    }

    void OGLRenderDevice::setLut2Texture(u32 tex) {
        lut2TextureId = tex;
    }

    void OGLRenderDevice::setLutMix(float mix) {
        lutMix = glm_clamp(mix, 0, 1);
    }

    void OGLRenderDevice::enableLut(bool enable) {
        lutActive = enable;
    }

    void OGLRenderDevice::enableCrtFx(bool enable) {
        useCrt = enable;
    }

    SDL_Rect* OGLRenderDevice::getCurrentViewPort() {
        return &currentViewport;
    }

    void OGLRenderDevice::checkCompileErrors(GLuint shader, const char *type) {
        GLint success;
        GLchar infoLog[1024];
        if (strcmp("PROGRAM", type) != 0) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                SDL_Log("Shader compilation error: %s\n%s", type, infoLog);
                throw std::runtime_error("Shader compilation error");
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                SDL_Log("Shader program link error: %s\n%s", type, infoLog);
                throw std::runtime_error("Shader link error");
            }
        }
    }

    GLuint OGLRenderDevice::createShaderProgram(const char *vertexsrc, const char *fragmentsrc) {
        u32 vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexsrc, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentsrc, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // shader Program
        GLuint id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return id;
    }

    char *OGLRenderDevice::loadFile(const char *filename) {
        char *buffer = nullptr;
        long length;
        FILE *f = fopen(filename, "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            buffer = (char*) malloc((size_t) length + 1);
            memset(buffer, 0, (size_t) length + 1);
            if (buffer) {
                fread(buffer, 1, (size_t) length, f);
            }
            fclose(f);
        } else {
            throw std::runtime_error("Could not open file");
        }
        return buffer;
    }

    GLuint OGLRenderDevice::loadShaderProgram(const char *vertFilename, const char *fragFilename) {
        char *vertSrc = loadFile(vertFilename);
        char *fragSrc = loadFile(fragFilename);
        GLuint programId = createShaderProgram(vertSrc, fragSrc);
        free(vertSrc);
        free(fragSrc);
        return programId;
    }

    void OGLRenderDevice::loadShaderPrograms() {
        screenQuadProgramID = loadShaderProgram("shaders/screen_vertex.glsl",
                                                "shaders/screen_crt_fragment.glsl");
        uniformScreenTexture = glGetUniformLocation(screenQuadProgramID, "screenTexture");
        uniformLightTexture = glGetUniformLocation(screenQuadProgramID, "lightTexture");
        uniformUseCrt = glGetUniformLocation(screenQuadProgramID, "useCrt");
        uniformUseLight = glGetUniformLocation(screenQuadProgramID, "useLight");

        texQuadProgramID = loadShaderProgram("shaders/plain_vertex.glsl",
                                             "shaders/plain_fragment.glsl");
        // resolve uniform locations
        uniformModelLoc = glGetUniformLocation(texQuadProgramID, "model");
        uniformViewLoc = glGetUniformLocation(texQuadProgramID, "view");
        uniformProjLoc = glGetUniformLocation(texQuadProgramID, "proj");
        uniformUseTextureLoc = glGetUniformLocation(texQuadProgramID, "use_texture");
        uniformIsFontLoc = glGetUniformLocation(texQuadProgramID, "is_font");
        uniformTextureLoc = glGetUniformLocation(texQuadProgramID, "tex");
        uniformLut1Texture = glGetUniformLocation(texQuadProgramID, "lutTexture");
        uniformLut2Texture = glGetUniformLocation(texQuadProgramID, "lutTexture2");
        uniformLutMix = glGetUniformLocation(texQuadProgramID, "lutMix");
        uniformUseLut = glGetUniformLocation(texQuadProgramID, "useLut");
        uniformMatte = glGetUniformLocation(texQuadProgramID, "matte");
        uniformMatteColor = glGetUniformLocation(texQuadProgramID, "matteColor");
    }

    void OGLRenderDevice::enableAdditiveAlpha() {
        glBlendFunc(GL_ONE, GL_ONE);
    }

    void OGLRenderDevice::enableRegularAlpha() {
        // straight alpha
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OGLRenderDevice::enablePremultipliedAlpha() {
        // premultiplied alpha
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
}