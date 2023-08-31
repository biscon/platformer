//
// Created by bison on 25-11-22.
//

#include "RenderDebugSystem.h"
#include "../../util/string_util.h"
#include "../components/ActorComponent.h"
#include "../PolyUtil.h"
#include "../Utils.h"
#include "../components/TransformComponent.h"
#include "../components/PhysicComponent.h"
#include "../components/CollisionInfoComponent.h"
#include <SDL_log.h>

RenderDebugSystem::RenderDebugSystem(RenderCmdBuffer &buffer, Font &font, Camera& camera) : buffer(buffer), font(font), camera(camera) {
}


INTERNAL const char *getStateName(ActorState& state)
{
    switch(state)
    {
        case ActorState::Idle:
            return "IDLE";
        case ActorState::Move:
            return "MOVE";
        case ActorState::Punch:
            return "PUNCH";
        case ActorState::Shoot:
            return "SHOOT";
        case ActorState::Jump:
            return "JUMP";
        case ActorState::Kick:
            return "KICK";
        case ActorState::Hurt:
            return "HURT";
        case ActorState::Dead:
            return "DEAD";
        case ActorState::Ladder:
            return "LADDER";
        case ActorState::Prune:
            return "PRUNE";
        default:
            return "NONE";
    }
}

void RenderDebugSystem::tick(World *world, float deltaTime) {
    if(!isActive) {
        return;
    }
    mat4 trans = {};
    glm_mat4_identity(trans);
    vec3 transvec = {-camera.scrollX, -camera.scrollY, 0};
    glm_translate(trans, transvec);
    buffer.pushTransform(&trans);

    // render camera focus area
    buffer.pushRect(camera.focusArea, RED);


    for (Entity *ent : world->all(false)) {
        float fontSize = (float) font.getSize();
        float margin = 4.0f;
        float leftStart = -125.0f;

        auto sprite = ent->get<SpriteComponent>();
        auto actor = ent->get<ActorComponent>();
        auto transform = ent->get<TransformComponent>();
        auto physic = ent->get<PhysicComponent>();
        auto colInfo = ent->get<CollisionInfoComponent>();

        FloatRect bounds;
        bounds.zero();
        getBounds(ent, bounds);

        if (transform.isValid()) {
            auto rect = FloatRect(transform->pos.x - 5, transform->pos.y - 5,
                                  transform->pos.x + 5, transform->pos.y + 5);
            rect.round();
            buffer.pushRect(rect, Renderer::YELLOW);
        }

            // render sprite debug
        if(sprite.isValid()) {
            auto box = bounds;
            box.round();
            buffer.pushRect(box, Renderer::RED);
            if (transform.isValid()) {
                buffer.pushText(string_format("POS:   %.2f,%.2f %.0fx %.2f", transform->pos.x,
                                              transform->pos.y, transform->scale,
                                              transform->rotation), &font, box.right + margin,
                                box.top + fontSize, Renderer::WHITE);

            }

            if (physic.isValid()) {
                buffer.pushText(
                        string_format("VEL:   %.2f,%.2f", physic->velocity.x, physic->velocity.y),
                        &font, box.right + margin, box.top + (fontSize * 2), Renderer::WHITE);
            }
            if (actor.isValid() && sprite.isValid()) {
                //SDL_Log("Current state: %s", getStateName(actor->state));
                buffer.pushText(
                        string_format("Name:  %s", actor->name.c_str()),
                        &font, box.right + margin, box.top + (fontSize * 4), Renderer::WHITE);
                buffer.pushText(
                        string_format("State: %s", getStateName(actor->state)),
                        &font, box.right + margin, box.top + (fontSize * 5), Renderer::WHITE);
                auto curAnim = sprite->getCurrentAnim();
                if(curAnim) {
                    buffer.pushText(
                            string_format("Anim:  %s", curAnim->name.c_str()),
                            &font, box.right + margin, box.top + (fontSize * 6), Renderer::WHITE);
                    buffer.pushText(
                            string_format("Frame: %d", curAnim->curFrame),
                            &font, box.right + margin, box.top + (fontSize * 7), Renderer::WHITE);
                }
            }
            if (colInfo.isValid()) {
                buffer.pushText(string_format("left:       %s", colInfo->cur().left ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 1), Renderer::WHITE);
                buffer.pushText(string_format("right:      %s", colInfo->cur().right ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 2), Renderer::WHITE);
                buffer.pushText(string_format("above:      %s", colInfo->cur().above ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 3), Renderer::WHITE);
                buffer.pushText(string_format("below:      %s", colInfo->cur().below ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 4), Renderer::WHITE);
                buffer.pushText(string_format("climbSlope: %s", colInfo->cur().climbingSlope ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 5), Renderer::WHITE);
                buffer.pushText(string_format("descSlope:  %s", colInfo->cur().descendingSlope ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 6), Renderer::WHITE);
                buffer.pushText(string_format("slopeAngle: %.2f", radiansToDegrees(colInfo->cur().slopeAngle)), &font, box.left + leftStart,
                                box.top + (fontSize * 7), Renderer::WHITE);
                buffer.pushText(string_format("ascending:  %s", colInfo->cur().ascending ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 8), Renderer::WHITE);
                buffer.pushText(string_format("distGround: %.2f", colInfo->cur().distanceGround), &font, box.left + leftStart,
                                box.top + (fontSize * 9), Renderer::WHITE);
                buffer.pushText(string_format("onLadder:   %s", colInfo->cur().onLadder ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 10), Renderer::WHITE);
                buffer.pushText(string_format("onTopLad:   %s", colInfo->cur().onTopLadder ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 11), Renderer::WHITE);
                buffer.pushText(string_format("slidingDown %s", colInfo->cur().slidingDownMaxSlope ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 12), Renderer::WHITE);
                buffer.pushText(string_format("onDoor:     %s", colInfo->cur().onDoor ? "true" : "false"), &font, box.left + leftStart,
                                box.top + (fontSize * 13), Renderer::WHITE);

            }
        }
    }
    glm_mat4_identity(trans);
    buffer.pushTransform(&trans);
}
