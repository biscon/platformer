//
// Created by bison on 01-11-22.
//

#include <SDL_log.h>
#include <unordered_map>
#include <stdexcept>
#include "Font.h"
#include "../util/string_util.h"

#define EN 0x01
#define EN_START 0x00
#define EN_END   0xFF
#define KR 0x02
#define KR_START 0xAC00
#define KR_END   0xD7A3

namespace Renderer {
    FT_Library library;

    FontLibrary::FontLibrary() {
        i32 error;
        error = FT_Init_FreeType(&library);
        if(error != 0) {
            throw std::runtime_error("Could not initialize FreeType");
        }
    }

    FontLibrary::~FontLibrary() {
        FT_Done_FreeType(library);
    }

    Font::Font(IRenderDevice &renderDevice, const std::string& path, u32 size) {
        //FT_Face face;
        if (FT_New_Face(library, path.c_str(), 0, &face))
            throw std::runtime_error("Could not load font: " + path);
        if (FT_Set_Pixel_Sizes(face, 0, size))
            throw std::runtime_error("Could not set pixel size for font face");
        //this->face = face;
        this->size = size;
        SDL_Log("Creating font size %d", size);
        atlas = std::make_unique<Renderer::TextureAtlas>(renderDevice, 1024, 1024, Renderer::PixelFormat::GREYSCALE);
        loadGlyphs(EN_START, EN_END);
        atlas->packAndUpload(renderDevice);
    }

    Font::~Font() {
        if(face != nullptr) {
            FT_Done_Face(face);
        }
    }

    void Font::loadGlyphs(u32 cpStart, u32 cpEnd) {
        for (u32 cp = cpStart; cp <= cpEnd; cp += 1) {

            FT_UInt index = FT_Get_Char_Index(face, cp);
            FT_Error err = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
            if (err) {
                SDL_Log("Failed to load glyph for codepoint: 0x%x\n", cp);
                continue;
            }
            //FT_Glyph_Metrics metrics = font->face->glyph->metrics;


            u32 w = face->glyph->bitmap.width;
            u32 h = face->glyph->bitmap.rows;

            //SDL_Log("Glyph w,h %d,%d", w, h);

            auto pb = std::make_unique<PixelBuffer>(w, h, PixelFormat::GREYSCALE);

            u8* pixels = (u8*) pb->pixels;
            u8* src_buf = face->glyph->bitmap.buffer;

            for(i32 y = 0; y < h; ++y) {
                for(i32 x = 0; x < w; ++x) {
                    *pixels = src_buf[(y*w)+x];
                    pixels++;
                }
                //pixels += 2;
            }

            //w+=2;
            //h+=2;

            //memcpy(pb.pixels, dst_buf, (size_t) w*h);
            //SDL_Log("Padding: %s", pb.padding ? "true" : "false");
            u32 atlas_id = atlas->addImage(std::move(pb));
            Glyph ch = {
                    atlas_id,
                    (u32) (face->glyph->advance.x >> 6),
            };
            ch.size[0] = w;
            ch.size[1] = h;
            ch.bearing[0] = face->glyph->bitmap_left;
            ch.bearing[1] = face->glyph->bitmap_top;
            glyphs[cp] = ch;
        }
    }

    u32 Font::measureTextWidth(const std::string& text) {
        size_t len = text.size();
        u32 x = 0;
        for (int i=0; i < len;) {
            // TODO(Brett): if the character is null, attempt to fetch it from the font
            u32 cpLen;
            u32 cp = DecodeCodePoint(&cpLen, &text[i]);

            Glyph* c = getGlyph(cp);
            //assert(c != NULL);
            i += cpLen;
            if(c == nullptr)
                continue;

            x += c->advance;
        }
        return x;
    }
}