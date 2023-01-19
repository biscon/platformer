//
// Created by bison on 31-10-22.
//

#include <SDL_log.h>
#include <cassert>
#include "TextureAtlas.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

namespace Renderer {
    namespace {
        float remapFloat(float oMin, float oMax, float nMin, float nMax, float x){
//range check
            if( oMin == oMax) {
                SDL_Log("Warning: Zero input range");
                return -1;    }

            if( nMin == nMax){
                SDL_Log("Warning: Zero output range");
                return -1;        }

//check reversed input range
            bool reverseInput = false;
            float oldMin = MIN(oMin, oMax );
            float oldMax = MAX(oMin, oMax );
            if (oldMin == oMin)
                reverseInput = true;

//check reversed output range
            bool reverseOutput = false;
            float newMin = MIN(nMin, nMax );
            float newMax = MAX(nMin, nMax );
            if (newMin == nMin)
                reverseOutput = true;

            float portion = (x-oldMin)*(newMax-newMin)/(oldMax-oldMin);
            if (reverseInput)
                portion = (oldMax-x)*(newMax-newMin)/(oldMax-oldMin);

            float result = portion + newMin;
            if (reverseOutput)
                result = newMax - portion;

            return result;
        }
    }

    TextureAtlas::TextureAtlas(IRenderDevice &renderDevice, i32 width, i32 height, PixelFormat format) : renderDevice(renderDevice) {
        noRects = 0;
        this->width = width;
        this->height = height;
        nextEntryId = 1;
        this->format = format;
        this->isUploaded = false;
    }

    TextureAtlas::TextureAtlas(IRenderDevice &renderDevice, i32 cellW, i32 cellH, PixelBuffer &sheet) : renderDevice(renderDevice) {
        width = sheet.width;
        height = sheet.height;
        format = sheet.pixelFormat;
        nextEntryId = 0;
        i32 cols = (sheet.width / cellW);
        i32 rows = (sheet.height / cellH);
        noRects = cols * rows;


        for (i32 y = 0; y < rows; ++y) {
            for (i32 x = 0; x < cols; ++x) {

                auto id = nextEntryId++;

                FloatRect uvRect;
                uvRect.left = remapFloat(0, (float) width, 0, 1, (float) (x * cellW));
                uvRect.right = remapFloat(0, (float) width, 0, 1,
                                          (float) ((x * cellW) + cellW));
                // texture space flips y axis just to piss you off :)
                uvRect.top = remapFloat(0, (float) height, 0, 1, (float) (y * cellH));
                uvRect.bottom = remapFloat(0, (float) height, 0, 1,
                                           (float) ((y * cellH) + cellH));
                //entryTable.emplace(id, id, nullptr, uvRect);

                entryTable.insert(std::make_pair(id, TextureAtlasEntry(id, nullptr, uvRect)));
            }
        }

        if (format == PixelFormat::RGBA) {
            textureId = renderDevice.uploadTexture(&sheet, false);
        } else {
            textureId = renderDevice.uploadTextureGreyscale(&sheet, false);
        }

        isUploaded = true;
    }

    TextureAtlas::~TextureAtlas() {
        // delete texture
        if(isUploaded) {
            renderDevice.deleteTexture(&textureId);
        }

    }

    u32 TextureAtlas::addImageFromPNG(const std::string &filename, bool pad) {
        SDL_Log("Loading png %s", filename.c_str());
        auto id = nextEntryId++;
        FloatRect frect;
        auto pb = new PixelBuffer(filename, pad);
        entryTable.insert(
                std::make_pair(id, TextureAtlasEntry(id, std::unique_ptr<PixelBuffer>(pb), frect)));

        //hashtable_insert(atlas->entryTable, (HASHTABLE_U64) entry.id, &entry);

        stbrp_rect rect;
        rect.id = id;
        rect.w = (stbrp_coord) pb->width;
        rect.h = (stbrp_coord) pb->height;
        rect.was_packed = 0;
        rectStore.emplace_back(rect);
        noRects++;
        return id;
    }

    u32 TextureAtlas::addImage(std::unique_ptr<PixelBuffer> image) {

        auto id = nextEntryId++;
        FloatRect frect;
        auto const pb = &*image;
        entryTable.insert(std::make_pair(id, TextureAtlasEntry(id, std::move(image), frect)));

        stbrp_rect rect;
        rect.id = id;
        rect.w = (stbrp_coord) pb->width;
        rect.h = (stbrp_coord) pb->height;
        rect.was_packed = 0;
        rectStore.emplace_back(rect);
        noRects++;
        return id;
    }

    void TextureAtlas::packAndUpload(IRenderDevice &renderDevice) {
        assert(!isUploaded);
        stbrp_context context;
        memset(&context, 0, sizeof(stbrp_context));

        // TODO this might overflow the stack
        i32 nodeCount = width * 2;
        struct stbrp_node nodes[nodeCount];

        stbrp_init_target(&context, width, height, nodes, nodeCount);
        stbrp_rect *rects = rectStore.data();
        stbrp_pack_rects(&context, rects, (i32) rectStore.size());

        auto buffer = std::make_unique<PixelBuffer>((u32) width, (u32) height, format);

        float atlasWidth = width;
        float atlasHeight = height;

        for (const auto &cur_rect : rectStore) {
            TextureAtlasEntry *entry = &entryTable.at(cur_rect.id);
            if (!entry->pixelBuffer->padding) {
                entry->uvRect.left = ((cur_rect.x) / atlasWidth);
                entry->uvRect.right = ((cur_rect.x + cur_rect.w) / atlasWidth);
                entry->uvRect.top = ((cur_rect.y) / atlasHeight);
                entry->uvRect.bottom = ((cur_rect.y + cur_rect.h) / atlasHeight);
            } else {
                entry->uvRect.left = ((cur_rect.x + 1.0f) / atlasWidth);
                entry->uvRect.right = ((cur_rect.x + cur_rect.w - 1.0f) / atlasWidth);
                entry->uvRect.top = ((cur_rect.y + 1.0f) / atlasHeight);
                entry->uvRect.bottom = ((cur_rect.y + cur_rect.h - 1.0f) / atlasHeight);
            }

            //entry->uvRect.add(0.00000030f);

            buffer->copyFrom(*entry->pixelBuffer, UIntRect(0, 0, entry->pixelBuffer->width,
                                                           entry->pixelBuffer->height),
                             UIntPos((u32) cur_rect.x, (u32) cur_rect.y));
            entry->pixelBuffer = nullptr;
        }


        if (format == PixelFormat::RGBA) {
            textureId = renderDevice.uploadTexture(buffer.get(), true);
        } else {
            textureId = renderDevice.uploadTextureGreyscale(buffer.get(), true);
        }
        isUploaded = true;
    }

    void TextureAtlas::setUVRect(u32 id, FloatRect *rect) {
        *rect = entryTable.at(id).uvRect;
    }

    int32_t TextureAtlas::getWidth() const {
        return width;
    }

    int32_t TextureAtlas::getHeight() const {
        return height;
    }

    uint32_t TextureAtlas::getTextureId() const {
        return textureId;
    }

    bool TextureAtlas::isIsUploaded() const {
        return isUploaded;
    }
}