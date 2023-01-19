//
// Created by bison on 27-10-22.
//

#ifndef GAME_IRENDERDEVICE_H
#define GAME_IRENDERDEVICE_H

#include "PixelBuffer.h"
#include <string>

namespace Renderer {
    class IRenderDevice {
    public:
        virtual ~IRenderDevice() = default;
        virtual u32 uploadTextureGreyscale(PixelBuffer *pb, bool filtering) = 0;
        virtual u32 uploadTexture(PixelBuffer *pb, bool filtering) = 0;
        virtual u32 uploadPNGTexture(const std::string& filename, bool filtering) = 0;
        virtual void deleteTexture(u32 *tex) = 0;
        virtual void setLut1Texture(u32 tex) = 0;
        virtual void setLut2Texture(u32 tex) = 0;
        virtual void setLutMix(float mix) = 0;
        virtual void enableLut(bool enable) = 0;
        virtual void enableCrtFx(bool enable) = 0;
    };
}


#endif //GAME_IRENDERDEVICE_H
