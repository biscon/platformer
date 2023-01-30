//
// Created by bison on 22-11-22.
//

#include <SDL_log.h>
#include "RenderLevelSystem.h"
#include "../components/ActorComponent.h"
#include "../components/TerrainComponent.h"
#include "../components/PointLightComponent.h"
#include "../components/VerletMeshComponent.h"
#include "../PolyUtil.h"
#include "../components/LadderComponent.h"
#include "../components/FlickerEffectComponent.h"
#include "../components/GlowEffectComponent.h"
#include <algorithm>

void RenderLevelSystem::tick(World *world, float deltaTime) {

    scroller.update(camera.scrollX, camera.scrollY);
    scroller.render(buffers.background, false);

    for (Entity* ent : world->all(false)) {
        auto ladder = ent->get<LadderComponent>();
        if(ladder.isValid()) {
            renderLadder(ent);
        }
    }

    // render layer sorted entities
    std::vector<Entity*> renderList;
    world->each<TransformComponent>([&](Entity* ent, ComponentHandle<TransformComponent> transform) {
        renderList.push_back(ent);
    });

    std::sort(renderList.begin(), renderList.end(),
         [](Entity* a, Entity* b) -> bool {
        return a->get<TransformComponent>()->layer < b->get<TransformComponent>()->layer;
    });

    for(Entity *ent : renderList) {
        auto transform = ent->get<TransformComponent>();
        auto terrain = ent->get<TerrainComponent>();
        auto image = ent->get<ImageComponent>();
        auto sprite = ent->get<SpriteComponent>();
        auto actor = ent->get<ActorComponent>();
        auto light = ent->get<PointLightComponent>();
        auto verlet = ent->get<VerletMeshComponent>();
        auto flicker = ent->get<FlickerEffectComponent>();
        bool on = true;
        if(flicker.isValid()) {
            on = flicker->on;
        }

        if(terrain.isValid()) {
            renderTerrain(ent);
        }

        if(image.isValid()) {
            if(on) {
                if (verlet.isValid()) {
                    renderVerletImage(ent, image->lightMap ? buffers.light : buffers.lit);
                } else {
                    renderImage(ent, image->lightMap ? buffers.light : buffers.lit);
                }
            }
        }
        if(actor.isValid() && sprite.isValid()) {
            sprite->horizFlip = actor->direction == ActorDirection::Left;
            renderSprite(sprite.get(), transform.get());
        }
        if(light.isValid()) {
            if(on) {
                renderLight(light.get(), transform.get());
            }
        }
        if(verlet.isValid()) {
            renderVerletMesh(verlet.get());
        }
    }

    for (Entity* ent : world->all(false)) {
        auto terrain = ent->get<TerrainComponent>();
        if(terrain.isValid()) {
            if(!terrain->light) {
                renderTerrainUnlight(ent);
            }
        }
    }


    // render foreground scrolling layers if any
    scroller.render(buffers.lit, true);
}


void RenderLevelSystem::renderSprite(SpriteComponent &sprite, TransformComponent &transform) {
    auto curAnim = sprite.getCurrentAnim();
    if(curAnim == nullptr) {
        return;
    }

    auto info = sprite.getFrameInfo();

    float ox,oy;
    sprite.getOrigin(&ox, &oy, 1);
    float fw = curAnim->animation->frameWidth;
    float fh = curAnim->animation->frameHeight;

    AtlasQuad quad = {.color = {sprite.alpha, sprite.alpha, sprite.alpha, sprite.alpha}, .atlasId = info->id,
            .left = transform.pos.x - ox,
            .top = transform.pos.y - oy,
            .right = transform.pos.x - ox + fw,
            .bottom = transform.pos.y - oy + fh};

    float xoff = transform.pos.x;
    float yoff = transform.pos.y;

    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {xoff, yoff, 0};
    glm_translate(trans, transvec);

    vec3 transvec3 = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec3);

    //glm_scale(trans, (vec3) {1.0f, 2.0f, 1.0});
    glm_rotate_z(trans, glm_rad(transform.rotation), trans);

    vec3 scalevec = {sprite.horizFlip ? -transform.scale : transform.scale, sprite.vertFlip ? -transform.scale : transform.scale, 1.0};
    glm_scale(trans, scalevec);

    vec3 transvec2 = {-xoff, -yoff, 0};
    glm_translate(trans, transvec2);


    buffers.lit.pushTransform(&trans);
    buffers.lit.pushAtlasQuad(quad, curAnim->animation->textureAtlas.get());
    glm_mat4_identity(trans);
    buffers.lit.pushTransform(&trans);
}

void RenderLevelSystem::renderTerrain(Entity* ent) {
    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec);
    buffers.lit.pushTransform(&trans);

    Color hidden = DARK_GREY;
    hidden.a = 0.3f;

    auto terrain = ent->get<TerrainComponent>();
    if(terrain.isValid()) {
        if (terrain->show) {
            buffers.lit.pushTriangles(terrain->triangleMesh, terrain->color);
        } else {
            if(editMode) {
                buffers.lit.pushTriangles(terrain->triangleMesh, hidden);
            }
        }
    }

    glm_mat4_identity(trans);
    buffers.lit.pushTransform(&trans);
}

void RenderLevelSystem::renderTerrainUnlight(Entity* ent) {
    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec);
    buffers.light.pushTransform(&trans);

    auto terrain = ent->get<TerrainComponent>();
    buffers.light.pushTriangles(terrain->triangleMesh, config.ambientColor);

    glm_mat4_identity(trans);
    buffers.light.pushTransform(&trans);
}

void RenderLevelSystem::renderLadder(Entity *ent) {
    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec);
    buffers.lit.pushTransform(&trans);

    auto ladder = ent->get<LadderComponent>();
    if(ladder.isValid()) {
        Quad q = { .color = BLUE};
        q.color.a = 0.5f;
        q.setBounds(ladder->rect);
        buffers.lit.pushQuad(q);
    }

    glm_mat4_identity(trans);
    buffers.lit.pushTransform(&trans);
}

void RenderLevelSystem::renderImage(Entity* ent, RenderCmdBuffer& buffer) {
    auto image = ent->get<ImageComponent>().get();
    auto transform = ent->get<TransformComponent>().get();
    auto glowEffect = ent->get<GlowEffectComponent>();
    float alpha = image.alpha;
    if(glowEffect.isValid()) {
        alpha = glowEffect->fraction;
    }

    Quad quad = {.color = {alpha, alpha, alpha, alpha},
            .left = transform.pos.x,
            .top = transform.pos.y,
            .right = transform.pos.x + image.width,
            .bottom = transform.pos.y + image.height};

    float xoff = transform.pos.x;
    float yoff = transform.pos.y;

    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {xoff, yoff, 0};
    glm_translate(trans, transvec);

    vec3 transvec3 = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec3);

    //glm_scale(trans, (vec3) {1.0f, 2.0f, 1.0});
    glm_rotate_z(trans, glm_rad(transform.rotation), trans);

    vec3 scalevec = {transform.scale, transform.scale, 1.0};
    glm_scale(trans, scalevec);

    vec3 transvec2 = {-xoff, -yoff, 0};
    glm_translate(trans, transvec2);


    buffer.pushTransform(&trans);
    buffer.pushTexturedQuad(quad, image.textureId);
    glm_mat4_identity(trans);
    buffer.pushTransform(&trans);
}

void RenderLevelSystem::renderLight(PointLightComponent &light, TransformComponent& transform) {

    float xoff = transform.pos.x;
    float yoff = transform.pos.y;

    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {xoff, yoff, 0};
    glm_translate(trans, transvec);

    vec3 transvec3 = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec3);

    //glm_scale(trans, (vec3) {1.0f, 2.0f, 1.0});
    //glm_rotate_z(trans, glm_rad(transform.rotation), trans);

    //vec3 scalevec = {transform.scale, transform.scale, 1.0};
    //glm_scale(trans, scalevec);

    //vec3 transvec2 = {-xoff, -yoff, 0};
    //glm_translate(trans, transvec2);

    buffers.light.pushTransform(&trans);
    buffers.light.pushTriangleStripMesh(light.innerMesh);
    buffers.light.pushTriangleStripMesh(light.outerMesh);
    glm_mat4_identity(trans);
    buffers.light.pushTransform(&trans);

    /*
    if(editMode) {
        buffers.unlit.pushTransform(&trans);
        buffers.unlit.pushTriangleStripMesh(light.innerMesh);
        buffers.unlit.pushTriangleStripMesh(light.outerMesh);
        glm_mat4_identity(trans);
        buffers.unlit.pushTransform(&trans);
    } else {
        buffers.light.pushTransform(&trans);
        buffers.light.pushTriangleStripMesh(light.innerMesh);
        buffers.light.pushTriangleStripMesh(light.outerMesh);
        glm_mat4_identity(trans);
        buffers.light.pushTransform(&trans);
    }
    */
}

void RenderLevelSystem::renderVerletMesh(VerletMeshComponent &verlet) {
    float pointRadius = 5;
    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec);
    buffers.unlit.pushTransform(&trans);

    for(auto &point : verlet.points) {
        Vector2 pos(point.position.x, point.position.y);
        buffers.unlit.pushCircle(pos, pointRadius, 16, point.locked ? RED : GREEN);
    }
    for(auto& stick : verlet.sticks) {
        Vector2 posA = verlet.points[stick.indexA].position;
        Vector2 posB = verlet.points[stick.indexB].position;
        buffers.unlit.pushLine(posA, posB, WHITE);
    }

    glm_mat4_identity(trans);
    buffers.unlit.pushTransform(&trans);
}

void RenderLevelSystem::renderVerletImage(Entity *ent, RenderCmdBuffer& buffer) {
    auto transform = ent->get<TransformComponent>();
    auto image = ent->get<ImageComponent>();
    auto verlet = ent->get<VerletMeshComponent>();
    auto glow = ent->get<GlowEffectComponent>();
    
    mat4 trans = {};
    glm_mat4_identity(trans);

    vec3 transvec3 = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec3);

    buffer.pushTransform(&trans);
    buffers.unlit.pushTransform(&trans);

    float imageW = image->width * transform->scale;
    float imageH = image->height * transform->scale;

    Vector2 topLeft = verlet->topLeft;
    Vector2 topRight = verlet->topRight;
    Vector2 bottomLeft = verlet->bottomLeft;
    Vector2 bottomRight = verlet->bottomRight;

    extendLineDirection(bottomLeft, topLeft, -verlet->attachOffset.y);
    extendLineDirection(bottomRight, topRight, -verlet->attachOffset.y);

    float horizExtension = fabsf((imageW/2) - ((verlet->topRight.x - verlet->topLeft.x)/2));
    extendLine(topLeft, topRight, horizExtension);
    //buffers.unlit.pushLine(topLeft, topRight, RED);

    extendLineDirection(verlet->topLeft, bottomLeft, imageH - 200);
    extendLineDirection(verlet->topRight, bottomRight, imageH - 200);
    extendLine(bottomLeft, bottomRight, horizExtension);

    //buffers.unlit.pushLine(bottomLeft, bottomRight, RED);

    std::vector<Vector2> rect;
    rect.emplace_back(topLeft);
    rect.emplace_back(topRight);
    rect.emplace_back(bottomRight);
    rect.emplace_back(bottomLeft);

    Color c = WHITE;
    if(glow.isValid()) {
        c = { glow->fraction, glow->fraction, glow->fraction, glow->fraction};
    }
    buffer.pushQuadMappedImage(rect, image->textureId, c);


    glm_mat4_identity(trans);
    buffer.pushTransform(&trans);
    buffers.unlit.pushTransform(&trans);
}