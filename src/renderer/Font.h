//
// Created by bison on 01-11-22.
//

#ifndef GAME_FONT_H
#define GAME_FONT_H


#include <defs.h>
#include <unordered_map>
#include "TextureAtlas.h"

extern "C" {
    #include <ft2build.h>
    #include FT_FREETYPE_H
};

namespace Renderer {
    typedef struct                  Glyph Glyph;

    struct Glyph {
        u32                         atlasId;
        u32                         advance;
        i32                         size[2];
        i32                         bearing[2];
    };

    typedef struct                  FT_FaceRec_* FT_Face;

    class FontLibrary {
    public:
        FontLibrary();
        ~FontLibrary();
    };

    class Font {
    public:
        Font(IRenderDevice &renderDevice, const std::string& path, u32 size);
        ~Font();

        u32 getSize() const {
            return size;
        }

        const std::unique_ptr<TextureAtlas> &getAtlas() const {
            return atlas;
        }

        Glyph* getGlyph(u32 code) {
            return &glyphs[code];
        }

        u32 measureTextWidth(const std::string& text);


    private:
        FT_Face face;
        u32 size;
        std::unique_ptr<TextureAtlas> atlas;
        std::unordered_map<u32, Glyph> glyphs;

    private:

        void loadGlyphs(u32 cpStart, u32 cpEnd);
    };
}


#endif //GAME_FONT_H
