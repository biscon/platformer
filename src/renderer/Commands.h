//
// Created by bison on 27-10-22.
//

#ifndef GAME_COMMANDS_H
#define GAME_COMMANDS_H

#include <cstddef>
#include <defs.h>
#include "Types.h"
#include "TextureAtlas.h"

extern "C" {
    #include <cglm/call/vec4.h>
}

namespace Renderer {


    // members private to Renderer namespace
    namespace {

        enum class CommandType {
            Clear,
            Quad,
            TexQuad,
            TexQuadAtlas,
            Transform,
            LineLoop,
            TriangleStripMesh,
            Triangles,
            EnableMatte,
            DisableMatte,
            EnableWind,
            DisableWind,
        };

        struct Command {
            CommandType type;
        };

        struct ClearCommand {
            CommandType type;
            Color color;
        };

        struct QuadCommand {
            CommandType type;
            size_t offset;
            size_t vertexOffset;
            size_t vertexCount;
            u32 texId;
            TextureAtlas *atlas;
            bool premultipliedAlpha = true;
        };

        struct EnableMatteCommand {
            CommandType type;
            Color color;
        };

        struct DisableMatteCommand {
            CommandType type;
        };

        struct TransformCommand {
            CommandType type;
            mat4 matrix;
        };

        struct VerticesCommand {
            CommandType type;
            size_t offset;
            size_t vertexOffset;
            size_t vertexCount;
        };

        struct EnableWindCommand {
            CommandType type = CommandType::EnableWind;
            float speed = 1.0f;
            float minStrength = 0.1f;
            float maxStrength = 0.5f;
            float strengthScale = 100.0;
            float interval = 3.5;
            float detail = 1.0;
            float distortion = 0; // hint_range(0.0, 1.0);
            float heightOffset = 0.0;
        };

        struct DisableWindCommand {
            CommandType type;
        };
    }
}

#endif
