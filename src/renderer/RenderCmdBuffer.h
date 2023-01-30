//
// Created by bison on 27-10-22.
//

#ifndef GAME_RENDERCMDBUFFER_H
#define GAME_RENDERCMDBUFFER_H

#include <defs.h>
#include <string>
#include <Vector2.h>
#include "Commands.h"
#include "Types.h"
#include "Font.h"

namespace Renderer {
    const size_t FLOATS_PER_VERTEX = 8;
    const size_t VERTS_PER_QUAD = 6;
    const size_t CMD_BUF_SIZE = KILOBYTES(1024);
    const size_t QUAD_VERTEX_BUF_SIZE = MEGABYTES(8);

    class RenderCmdBuffer {
    public:
        RenderCmdBuffer();
        void clear();

        void pushClear(Color color);
        void pushAtlasQuad(const AtlasQuad& q, TextureAtlas *atlas);
        void pushAtlasQuads(const std::vector<AtlasQuad>& quads, TextureAtlas *atlas);
        void pushQuads(const std::vector<Quad>& quads);
        void pushQuad(const Quad& q);
        void pushTexturedQuads(const std::vector<Quad>& quads, u32 texid);
        void pushTexturedQuad(const Quad& q, u32 texid);

        void pushTransform(mat4* matrix);
        void pushText(const std::string& text, Font *font, float x, float y, const Color& color);


        void pushRect(FloatRect& rect, const Color& color);
        void pushLine(const Vector2& from, const Vector2& to, const Color& color);
        void pushCircle(Vector2 &pos, float radius, u32 pts, const Color &color);
        void pushEnableMatte(const Color& color);
        void pushDisableMatte();

        void pushTriangles(const std::vector<Vector2> &verts, const Color &color);
        void pushTriangleStripMesh(const std::vector<float> &verts);

        void pushQuadMappedImage(const std::vector<Vector2> &verts, TextureAtlas* atlas, u32 atlasId, const Color& color);
        void pushQuadMappedImage(const std::vector<Vector2> &rect, u32 textureId, const Color& color);


        u8 *commands;
        u8 *vertices;
        u8 *cmdOffset;
        u8 *verticesOffset;
    private:

    };
}

#endif //GAME_RENDERCMDBUFFER_H
