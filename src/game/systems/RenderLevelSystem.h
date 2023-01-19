//
// Created by bison on 22-11-22.
//

#ifndef GAME_SPRITESYSTEM_H
#define GAME_SPRITESYSTEM_H
#include <ECS.h>
#include <RenderBuffers.h>
#include <LevelConfig.h>
#include "../components/SpriteComponent.h"
#include "../../renderer/RenderCmdBuffer.h"
#include "../Camera.h"
#include "../components/ImageComponent.h"
#include "../ParallaxScroller.h"
#include "../components/PointLightComponent.h"
#include "../components/VerletMeshComponent.h"
#include "../components/TransformComponent.h"

extern "C" {
    #include <cglm/call/vec4.h>
}

using namespace ECS;
using namespace Renderer;

class RenderLevelSystem : public EntitySystem {
public:
    explicit RenderLevelSystem(RenderBuffers buffers, Camera& camera, ParallaxScroller& scroller, LevelConfig& config) : buffers(buffers), camera(camera), scroller(scroller), config(config) {}
    ~RenderLevelSystem() override = default;
    void tick(World* world, float deltaTime) override;
    bool editMode = false;

private:
    RenderBuffers buffers;
    Camera& camera;
    ParallaxScroller& scroller;
    LevelConfig& config;

    void renderSprite(SpriteComponent &sprite, TransformComponent &transform);
    void renderTerrain(Entity* ent);
    void renderTerrainUnlight(Entity* ent);
    void renderLadder(Entity* ent);
    void renderImage(Entity *ent, RenderCmdBuffer& buffer);
    void renderLight(PointLightComponent &light, TransformComponent& transform);
    void renderVerletMesh(VerletMeshComponent& verlet);
    void renderVerletImage(Entity* ent, RenderCmdBuffer& buffer);
};


#endif //GAME_SPRITESYSTEM_H
