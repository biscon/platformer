//
// Created by bison on 31-10-22.
//

#ifndef GAME_TEXTUREATLAS_H
#define GAME_TEXTUREATLAS_H

#include <defs.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Types.h"
#include "PixelBuffer.h"
#include "IRenderDevice.h"
#include <stb_rect_pack.h>

namespace Renderer {
    struct TextureAtlasEntry {
        TextureAtlasEntry(u32 id, std::unique_ptr<PixelBuffer> pixelBuffer,
                          const FloatRect &uvRect) : id(id), pixelBuffer(std::move(pixelBuffer)),
                                                     uvRect(uvRect) {}

        u32 id;
        std::unique_ptr<PixelBuffer> pixelBuffer;
        FloatRect uvRect;
    };


    class TextureAtlas {
    public:
        TextureAtlas(IRenderDevice &renderDevice, i32 width, i32 height, PixelFormat format);
        TextureAtlas(IRenderDevice &renderDevice, i32 cellW, i32 cellH, PixelBuffer& sheet);
        ~TextureAtlas();

        u32 addImageFromPNG(const std::string &filename, bool pad);
        u32 addImage(std::unique_ptr<PixelBuffer> pb);
        void packAndUpload(IRenderDevice &renderDevice);
        void setUVRect(u32 id, FloatRect* rect);

        i32 getWidth() const;
        i32 getHeight() const;
        uint32_t getTextureId() const;
        bool isIsUploaded() const;
        PixelFormat format;

        i32 getNoImages() {
            return entryTable.size();
        }

    private:
        IRenderDevice &renderDevice;
        std::unordered_map<u32, TextureAtlasEntry> entryTable;
        std::vector<stbrp_rect> rectStore;
        i32 noRects;
        i32 width;
        i32 height;
        u32 nextEntryId;
        u32 textureId;
        bool isUploaded;
    };
}



#endif //GAME_TEXTUREATLAS_H
