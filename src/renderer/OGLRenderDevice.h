//
// Created by bison on 27-10-22.
//

#ifndef GAME_GLRENDERER_H
#define GAME_GLRENDERER_H

extern "C" {
    #include <glad.h>
};

#include <memory>
#include <defs.h>
#include "RenderCmdBuffer.h"
#include "IRenderDevice.h"
#include "PixelBuffer.h"
#include <SDL_rect.h>

namespace Renderer {
    class OGLRenderDevice: public IRenderDevice {
    public:
        OGLRenderDevice(i32 width, i32 height);
        ~OGLRenderDevice() override;

        u32 uploadTexture(PixelBuffer *pb, bool filtering) override;
        u32 uploadPNGTexture(const std::string& filename, bool filtering) override;
        u32 uploadTextureGreyscale(PixelBuffer *pb, bool filtering) override;

        void deleteTexture(u32 *tex) override;
        void viewportSizeChanged(i32 newWidth, i32 newHeight);
        void setLut1Texture(u32 tex) override;
        void setLut2Texture(u32 tex) override;
        void setLutMix(float mix) override;
        void enableLut(bool enable) override;
        void enableCrtFx(bool enable) override;
        SDL_Rect *getCurrentViewPort();
        void renderLitBuffer(RenderCmdBuffer &buf);
        void renderUnlitBuffer(RenderCmdBuffer &buf);
        void renderBackgroundBuffer(RenderCmdBuffer &buf);
        void renderLightBuffer(RenderCmdBuffer &buf);
        void renderScreen();
        void setEngineTime(float time);

    private:
        void renderBuffer(RenderCmdBuffer &buf, GLuint fbo, bool useLut);
        void checkCompileErrors(GLuint shader, const char *type);
        GLuint createShaderProgram(const char *vertexsrc, const char *fragmentsrc);
        char *loadFile(const char *filename);
        GLuint loadShaderProgram(const char *vertFilename, const char *fragFilename);
        void loadShaderPrograms();
        void enableRegularAlpha();
        void enablePremultipliedAlpha();
        void enableAdditiveAlpha();
        void renderCmdBuffer(RenderCmdBuffer &buf);


        GLuint texQuadVBO;
        GLuint texQuadVAO;
        GLuint bgFBO;
        GLuint bgTextureId;
        GLuint litFBO;
        GLuint litTextureId;
        GLuint unlitFBO;
        GLuint unlitTextureId;
        GLuint lightFBO;
        GLuint lightTextureId;
        GLuint screenQuadVBO;
        GLuint screenQuadVAO;
        GLuint screenQuadProgramID;
        GLint uniformScreenTexture;
        GLint uniformLightTexture;
        GLint uniformUseLight;

        GLuint texQuadProgramID;
        GLint uniformModelLoc;
        GLint uniformViewLoc;
        GLint uniformProjLoc;
        GLint uniformUseTextureLoc;
        GLint uniformTextureLoc;

        GLint uniformIsFontLoc;
        i32 screenWidth;
        i32 screenHeight;
        SDL_Rect currentViewport;

        GLint uniformLut1Texture;
        GLint uniformLut2Texture;
        u32 lut1TextureId;
        u32 lut2TextureId;
        GLint uniformLutMix;
        GLint uniformUseLut;
        float lutMix = 0;

        GLint uniformUseCrt;
        bool useCrt = false;
        bool lutActive = false;
        GLint uniformMatte;
        GLint uniformMatteColor;

        GLint uniformEngineTime = 0;
        float engineTime = 0;

        GLint uniformUseWind = 0;
        GLint uniformWindSpeed;
        GLint uniformWindMinStrength;
        GLint uniformWindMaxStrength;
        GLint uniformWindStrengthScale;
        GLint uniformWindInterval;
        GLint uniformWindDetail;
        GLint uniformWindDistortion;
        GLint uniformWindHeightOffset;
    };
    
}


#endif //GAME_GLRENDERER_H
