//
// Created by bison on 27-10-22.
//

#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "RenderCmdBuffer.h"
#include "../util/string_util.h"
#include "../util/math_util.h"

namespace Renderer {
    RenderCmdBuffer::RenderCmdBuffer() {
        commands = (u8 *) calloc(1, CMD_BUF_SIZE);
        vertices = (u8 *) calloc(1, QUAD_VERTEX_BUF_SIZE);
        cmdOffset = commands;
        verticesOffset = vertices;
    }

    void RenderCmdBuffer::clear() {
        memset(commands, 0, CMD_BUF_SIZE);
        memset(vertices, 0, QUAD_VERTEX_BUF_SIZE);
        cmdOffset = commands;
        verticesOffset = vertices;
    }

    void RenderCmdBuffer::pushClear(const Renderer::Color color) {
        ClearCommand cmd = {
                .type = CommandType::Clear,
                .color = color
        };
        memcpy(cmdOffset, &cmd, sizeof(ClearCommand));
        cmdOffset += sizeof(ClearCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushAtlasQuad(const AtlasQuad& q, TextureAtlas *atlas) {
        QuadCommand cmd = {
                .type = CommandType::TexQuadAtlas,
                .offset = 0,
                .vertexOffset = 0,
                .vertexCount = 0,
                .texId = atlas->getTextureId(),
                .atlas = atlas
        };
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = VERTS_PER_QUAD;

        FloatRect uv;
        atlas->setUVRect(q.atlasId, &uv);

        float verts[] = {
                // positions            // colors                                           // texture coords
                q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.right,uv.top,         // right top
                q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right,uv.bottom,      // right bottom
                q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left,uv.top,          // left top
                q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right,uv.bottom,      // right bottom
                q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.left,uv.bottom,       // left bottom
                q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left,uv.top           // left top
        };

        memcpy(verticesOffset, &verts, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }


    void RenderCmdBuffer::pushAtlasQuads(const std::vector<AtlasQuad>& quads, TextureAtlas *atlas) {
        QuadCommand cmd = {
                .type = CommandType::TexQuadAtlas,
                .offset = 0,
                .vertexOffset = 0,
                .vertexCount = 0,
                .texId = atlas->getTextureId(),
                .atlas = atlas
        };
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = quads.size() * VERTS_PER_QUAD;

        for(auto& q : quads) {
            FloatRect uv;
            atlas->setUVRect(q.atlasId, &uv);

            float verts[] = {
                    // positions            // colors                                           // texture coords
                    q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.right,uv.top,         // right top
                    q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right,uv.bottom,      // right bottom
                    q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left,uv.top,          // left top
                    q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right,uv.bottom,      // right bottom
                    q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.left,uv.bottom,       // left bottom
                    q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left,uv.top           // left top
            };

            memcpy(verticesOffset, &verts, byte_size);
            verticesOffset += byte_size;
            assert(verticesOffset - vertices > 0);
        }

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushQuads(const std::vector<Quad>& quads) {
        QuadCommand cmd = {
                .type = CommandType::Quad,
        };

        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = quads.size() * VERTS_PER_QUAD;
        
        for(auto& q : quads) {
            float verts[] = {
                    // positions            // colors                                           // texture coords
                    q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,
                    1.0f,   // top right
                    q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,
                    0.0f,   // bottom right
                    q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,
                    1.0f,   // top left

                    q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,
                    0.0f,   // bottom right
                    q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,
                    0.0f,   // bottom left
                    q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,
                    1.0f    // top left
            };

            memcpy(verticesOffset, &verts, byte_size);
            verticesOffset += byte_size;
            assert(verticesOffset - vertices > 0);
        }

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushQuad(const Quad& q) {
        QuadCommand cmd = {
                .type = CommandType::Quad,
        };

        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = VERTS_PER_QUAD;

        float verts[] = {
                // positions            // colors                                           // texture coords
                q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,
                1.0f,   // top right
                q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,
                0.0f,   // bottom right
                q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,
                1.0f,   // top left

                q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,
                0.0f,   // bottom right
                q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,
                0.0f,   // bottom left
                q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,
                1.0f    // top left
        };

        memcpy(verticesOffset, &verts, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushTexturedQuads(const std::vector<Quad>& quads, u32 texid) {
        QuadCommand cmd = {
                .type = CommandType::TexQuad,
                .offset = 0,
                .vertexOffset = 0,
                .vertexCount = 0,
                .texId = texid,
                .atlas = nullptr
        };

        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = quads.size() * VERTS_PER_QUAD;

        for(auto& q : quads) {
            float verts[] = {
                    // positions            // colors                     // texture coords
                    q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,0.0f,   // top right
                    q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f,   // bottom right
                    q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f,   // top left

                    q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f,   // bottom right
                    q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,1.0f,   // bottom left
                    q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f    // top left
            };

            memcpy(verticesOffset, &verts, byte_size);
            verticesOffset += byte_size;
            assert(verticesOffset - (vertices) > 0);
        }

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushTexturedQuad(const Quad& q, u32 texid) {
        QuadCommand cmd = {
                .type = CommandType::TexQuad,
                .offset = 0,
                .vertexOffset = 0,
                .vertexCount = 0,
                .texId = texid,
                .atlas = nullptr
        };

        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = VERTS_PER_QUAD;


        float verts[] = {
                // positions            // colors                     // texture coords
                q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,0.0f,   // top right
                q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f,   // bottom right
                q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f,   // top left

                q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f,   // bottom right
                q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,1.0f,   // bottom left
                q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f    // top left
        };

        memcpy(verticesOffset, &verts, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - (vertices) > 0);


        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushTransform(mat4 *matrix) {
        TransformCommand cmd = {.type = CommandType::Transform};
        memcpy(&cmd.matrix, matrix, sizeof(mat4));
        memcpy(cmdOffset, &cmd, sizeof(TransformCommand));
        cmdOffset += sizeof(TransformCommand);
        assert(cmdOffset - commands > 0);

    }

    void RenderCmdBuffer::pushText(const std::string& text, Font *font, float x, float y,
                                             const Color& color) {
        size_t len = text.size();
        std::vector<AtlasQuad> quads;

        for (size_t i = 0; i < len;) {
            // TODO(Brett): if the character is null, attempt to fetch it from the font
            u32 cpLen;
            u32 cp = DecodeCodePoint(&cpLen, &text[i]);

            const Glyph *c = font->getGlyph(cp);

            //assert(c != NULL);
            i += cpLen;
            if (c == nullptr)
                continue;
            float xp, yp, h, w;
            xp = x + c->bearing[0];
            yp = y - c->bearing[1];
            w = c->size[0];
            h = c->size[1];
            AtlasQuad quad = {
                    .color      = {color.r, color.g, color.b, color.a},
                    .atlasId    = c->atlasId,
                    .left       = xp,
                    .top        = yp,
                    .right      = xp + w,
                    .bottom     = yp + h
            };
            quads.emplace_back(quad);
            x += c->advance;
        }

        //SDL_Log("Generated %d glyph quads", quadStore.noItems);
        pushAtlasQuads(quads, font->getAtlas().get());
    }

    void RenderCmdBuffer::pushRect(FloatRect &rect, const Color &color) {
        VerticesCommand cmd = {.type = CommandType::LineLoop};
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = 4 * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = 4;
        

        float vertex[] = {
                // positions                // colors                               // texture coords
                rect.left, rect.top,        color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
                rect.right, rect.top,       color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
                rect.right, rect.bottom,    color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
                rect.left, rect.bottom,     color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
        };

        memcpy(verticesOffset, &vertex, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(VerticesCommand));
        cmdOffset += sizeof(VerticesCommand);
    }

    void RenderCmdBuffer::pushLine(const Vector2 &from, const Vector2 &to, const Color &color) {
        VerticesCommand cmd = {.type = CommandType::LineLoop};
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = 2 * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = 2;
        
        float vertex[] = {
                // positions                // colors                               // texture coords
                from.x, from.y,        color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
                to.x, to.y,       color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
        };

        memcpy(verticesOffset, &vertex, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(VerticesCommand));
        cmdOffset += sizeof(VerticesCommand);
    }

    void RenderCmdBuffer::pushCircle(Vector2 &pos, float radius, u32 pts, const Color &color) {
        VerticesCommand cmd = {.type = CommandType::LineLoop};
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = 1 * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = pts;

        auto fpts = (float) pts;
        for(u32 i = 1; i <= pts; i += 1)
        {
            float angle = ( i / fpts ) * PI * 2.0f;
            Vector2 v0(radius * cosf(angle), radius * sinf(angle));
            v0 += pos;
            float vertex[] = {
                    // positions                // colors                               // texture coords
                    v0.x, v0.y,        color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
            };
            memcpy(verticesOffset, &vertex, byte_size);
            verticesOffset += byte_size;
            assert(verticesOffset - vertices > 0);

        }

        //Vector2 v0(vertices[0], vertices[1]);

        memcpy(cmdOffset, &cmd, sizeof(VerticesCommand));
        cmdOffset += sizeof(VerticesCommand);
    }

    void RenderCmdBuffer::pushEnableMatte(const Color &color) {
        EnableMatteCommand cmd = {.type = CommandType::EnableMatte, .color = color};
        memcpy(cmdOffset, &cmd, sizeof(EnableMatteCommand));
        cmdOffset += sizeof(EnableMatteCommand);
        assert(cmdOffset - commands > 0); 
    }

    void RenderCmdBuffer::pushDisableMatte() {
        DisableMatteCommand cmd = {.type = CommandType::DisableMatte};
        memcpy(cmdOffset, &cmd, sizeof(DisableMatteCommand));
        cmdOffset += sizeof(DisableMatteCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushTriangleStripMesh(const std::vector<float> &verts) {
        VerticesCommand cmd = {.type = CommandType::TriangleStripMesh};
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = verts.size()/8;

        size_t totalByteSize = (verts.size()/8) * (byte_size);
        memcpy(verticesOffset, verts.data(), totalByteSize);
        verticesOffset += totalByteSize;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(VerticesCommand));
        cmdOffset += sizeof(VerticesCommand);
    }

    void RenderCmdBuffer::pushTriangles(const std::vector<Vector2> &verts, const Color &color) {
        VerticesCommand cmd = {.type = CommandType::Triangles};
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = verts.size();

        for(auto& v : verts) {
            float vertex[] = {
                    // positions                // colors                               // texture coords
                    v.x, v.y,        color.r, color.g,  color.b, color.a,    0.0f, 0.0f,
            };

            memcpy(verticesOffset, &vertex, byte_size);
            verticesOffset += byte_size;
            assert(verticesOffset - vertices > 0);
        }

        memcpy(cmdOffset, &cmd, sizeof(VerticesCommand));
        cmdOffset += sizeof(VerticesCommand);
    }

    void RenderCmdBuffer::pushQuadMappedImage(const std::vector<Vector2> &rect, TextureAtlas* atlas, u32 atlasId, const Color& color) {
        QuadCommand cmd = {
                .type = CommandType::TexQuadAtlas,
                .offset = 0,
                .vertexOffset = 0,
                .vertexCount = 0,
                .texId = atlas->getTextureId(),
                .atlas = atlas
        };
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = VERTS_PER_QUAD;
        //cmd.premultipliedAlpha = false;

        FloatRect uv;
        atlas->setUVRect(atlasId, &uv);

        float verts[] = {
                // positions            // colors                                           // texture coords
                rect[1].x, rect[1].y, color.r, color.g, color.b, color.a, uv.right,uv.top,         // right top
                rect[2].x, rect[2].y, color.r, color.g, color.b, color.a, uv.right,uv.bottom,      // right bottom
                rect[0].x, rect[0].y, color.r, color.g, color.b, color.a, uv.left,uv.top,          // left top
                rect[2].x, rect[2].y, color.r, color.g, color.b, color.a, uv.right,uv.bottom,      // right bottom
                rect[3].x, rect[3].y, color.r, color.g, color.b, color.a, uv.left,uv.bottom,       // left bottom
                rect[0].x, rect[0].y, color.r, color.g, color.b, color.a, uv.left,uv.top           // left top
        };

        memcpy(verticesOffset, &verts, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushQuadMappedImage(const std::vector<Vector2> &rect, u32 textureId, const Color& color) {
        QuadCommand cmd = {
                .type = CommandType::TexQuad,
                .offset = 0,
                .vertexOffset = 0,
                .vertexCount = 0,
                .texId = textureId,
                .atlas = nullptr
        };
        cmd.offset = (size_t) (verticesOffset - vertices);
        size_t byte_size = VERTS_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float);
        cmd.vertexOffset = (cmd.offset) / (FLOATS_PER_VERTEX * sizeof(float));
        cmd.vertexCount = VERTS_PER_QUAD;
        //cmd.premultipliedAlpha = false;

        float verts[] = {
                // positions            // colors                                           // texture coords
                rect[1].x, rect[1].y, color.r, color.g, color.b, color.a, 1.0f, 0.0f,         // right top
                rect[2].x, rect[2].y, color.r, color.g, color.b, color.a, 1.0f, 1.0f,      // right bottom
                rect[0].x, rect[0].y, color.r, color.g, color.b, color.a, 0.0f, 0.0f,          // left top
                rect[2].x, rect[2].y, color.r, color.g, color.b, color.a, 1.0f, 1.0f,      // right bottom
                rect[3].x, rect[3].y, color.r, color.g, color.b, color.a, 0.0f, 1.0f,       // left bottom
                rect[0].x, rect[0].y, color.r, color.g, color.b, color.a, 0.0f, 0.0f           // left top
        };

        memcpy(verticesOffset, &verts, byte_size);
        verticesOffset += byte_size;
        assert(verticesOffset - vertices > 0);

        memcpy(cmdOffset, &cmd, sizeof(QuadCommand));
        cmdOffset += sizeof(QuadCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushEnableWind(const WindEffectParameters &params) {
        EnableWindCommand cmd = {.type = CommandType::EnableWind, .speed = params.speed, .minStrength = params.minStrength, .maxStrength = params.maxStrength, .strengthScale = params.strengthScale, .interval = params.interval, .detail = params.detail, .distortion = params.distortion, .heightOffset = params.heightOffset};
        memcpy(cmdOffset, &cmd, sizeof(EnableWindCommand));
        cmdOffset += sizeof(EnableWindCommand);
        assert(cmdOffset - commands > 0);
    }

    void RenderCmdBuffer::pushDisableWind() {
        DisableWindCommand cmd = {.type = CommandType::DisableWind};
        memcpy(cmdOffset, &cmd, sizeof(DisableWindCommand));
        cmdOffset += sizeof(DisableWindCommand);
        assert(cmdOffset - commands > 0);
    }
}