//
// Created by bison on 25-11-22.
//

#ifndef PLATFORMER_RENDERDEBUGSYSTEM_H
#define PLATFORMER_RENDERDEBUGSYSTEM_H

#include <ECS.h>
#include "../components/SpriteComponent.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../Camera.h"

using namespace ECS;
using namespace Renderer;

class RenderDebugSystem : public EntitySystem {
public:
    explicit RenderDebugSystem(RenderCmdBuffer &buffer, Font& font, Camera& camera);
    ~RenderDebugSystem() override = default;
    void tick(World *world, float deltaTime) override;
    bool isActive = false;

private:
    RenderCmdBuffer& buffer;
    Font& font;
    Camera& camera;
};


#endif
