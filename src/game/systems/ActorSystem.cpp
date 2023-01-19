//
// Created by bison on 22-11-22.
//

#include <SDL_log.h>
#include <cglm/util.h>
#include "ActorSystem.h"
#include "../PolyUtil.h"

void ActorSystem::tick(World *world, float deltaTime) {
    for (Entity* ent : world->all(false)) {
        auto input = ent->get<InputComponent>();
        auto physic = ent->get<PhysicComponent>();
        auto colInfo = ent->get<CollisionInfoComponent>();
        auto actor = ent->get<ActorComponent>();
        auto sprite = ent->get<SpriteComponent>();
        if(!input.isValid() || !actor.isValid() || !physic.isValid() || !colInfo.isValid() || !sprite.isValid()) {
            continue;
        }
        switch(actor->state) {

            case ActorState::Idle:
                stateIdle(input.get(), physic.get(), colInfo.get(), actor.get(), sprite.get(), deltaTime);
                break;
            case ActorState::Move:
                stateMove(input.get(), physic.get(), colInfo.get(), actor.get(), sprite.get(), deltaTime);
                break;
            case ActorState::Jump:
                stateJump(input.get(), physic.get(), colInfo.get(), actor.get(), sprite.get(), deltaTime);
                break;
            case ActorState::Punch:break;
            case ActorState::Kick:break;
            case ActorState::Shoot:break;
            case ActorState::Hurt:break;
            case ActorState::Dead:break;
            case ActorState::Ladder: {
                stateLadder(input.get(), physic.get(), colInfo.get(), actor.get(), sprite.get(), deltaTime);
                break;
            }
            case ActorState::Prune:break;
        }
    }
}

void ActorSystem::stateIdle(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime) {
    // input
    if(input.cur().left == input.cur().right) {
        if(physic.velocity.x != 0) {
            float accTime = colInfo.cur().below ? physic.accTimeGrounded
                                                 : physic.accTimeAirborne;
            lerpVelocityX(physic, 0, accTime, deltaTime);
        }
    } else {
        actor.state = ActorState::Move;
        sprite.setAnimIfNotCur("walk");
    }

    if(input.cur().up && colInfo.cur().onLadder) {
        actor.state = ActorState::Ladder;
    }

    if(input.cur().down && colInfo.cur().onTopLadder) {
        actor.state = ActorState::Ladder;
    }

    if(input.cur().jump && (colInfo.cur().below || colInfo.cur().distanceGround <= maxJumpDistance)) {
        if(colInfo.cur().slidingDownMaxSlope) {
            float dirInput = actor.direction == ActorDirection::Right ? 1 : -1;
            if(dirInput != -glm_signf(colInfo.cur().slopeNormal.x)) {
                SDL_Log("jump while sliding slope idle");
                sprite.setAnimIfNotCur("ascend");
                actor.state = ActorState::Jump;
                physic.velocity.y = physic.maxJumpVelocity * colInfo.cur().slopeNormal.y;
                if(dirInput == 1) {
                    physic.velocity.x = -physic.maxJumpVelocity * colInfo.cur().slopeNormal.y;
                } else {
                    physic.velocity.x = physic.maxJumpVelocity * colInfo.cur().slopeNormal.y;
                }
            }
        } else {
            sprite.setAnimIfNotCur("ascend");
            actor.state = ActorState::Jump;
            physic.velocity.y = -physic.maxJumpVelocity;
        }
    } else if(!colInfo.cur().below && colInfo.cur().distanceGround > minFallDistance) {
        sprite.setAnimIfNotCur("descend");
        actor.state = ActorState::Jump;
    }

}

void ActorSystem::stateMove(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime) {
    // input
    if(input.cur().left == input.cur().right) {
        actor.state = ActorState::Idle;
        sprite.setAnimIfNotCur("idle");
    } else if(input.cur().right) {
        actor.direction = ActorDirection::Right;
        float accTime = colInfo.cur().below ? physic.accTimeGrounded : physic.accTimeAirborne;
        lerpVelocityX(physic, physic.moveSpeed, accTime, deltaTime);

    } else if(input.cur().left) {
        actor.direction = ActorDirection::Left;
        float accTime = colInfo.cur().below ? physic.accTimeGrounded : physic.accTimeAirborne;
        lerpVelocityX(physic, -physic.moveSpeed, accTime, deltaTime);
    }

    if(input.cur().jump && (colInfo.cur().below || colInfo.cur().distanceGround <= maxJumpDistance)) {
        if(colInfo.cur().slidingDownMaxSlope) {
            float dirInput = actor.direction == ActorDirection::Right ? 1 : -1;
            if(dirInput != -glm_signf(colInfo.cur().slopeNormal.x)) {
                SDL_Log("jump while sliding slope move");
                sprite.setAnimIfNotCur("ascend");
                actor.state = ActorState::Jump;
                physic.velocity.y = physic.maxJumpVelocity * colInfo.cur().slopeNormal.y;
                if(dirInput == 1) {
                    physic.velocity.x = -physic.maxJumpVelocity * colInfo.cur().slopeNormal.y;
                } else {
                    physic.velocity.x = physic.maxJumpVelocity * colInfo.cur().slopeNormal.y;
                }
            }
        } else {
            sprite.setAnimIfNotCur("ascend");
            actor.state = ActorState::Jump;
            physic.velocity.y = -physic.maxJumpVelocity;
        }
    } else if(!colInfo.cur().below && colInfo.cur().distanceGround > minFallDistance) {
        sprite.setAnimIfNotCur("descend");
        actor.state = ActorState::Jump;
    }
}

void ActorSystem::stateJump(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime) {

    if(!input.cur().jump && colInfo.cur().ascending) {
        if(physic.velocity.y < -physic.minJumpVelocity) {
            physic.velocity.y = -physic.minJumpVelocity;
            SDL_Log("setting velocity to min jump");
        }
    }

    if(colInfo.prev().ascending && !colInfo.cur().ascending && colInfo.cur().distanceGround > minFallDistance) {
        sprite.setAnimIfNotCur("descend");
    }

    if(colInfo.cur().below) {
        if(input.cur().left == input.cur().right) {
            sprite.setAnimIfNotCur("idle");
            actor.state = ActorState::Idle;
        } else {
            sprite.setAnimIfNotCur("walk");
            actor.state = ActorState::Move;
        }
    } else {
        if(input.cur().left == input.cur().right) {
            if(physic.velocity.x != 0) {
                float accTime = colInfo.cur().below ? physic.accTimeGrounded
                                                    : physic.accTimeAirborne;
                lerpVelocityX(physic, 0, accTime, deltaTime);
            }
        } else if(input.cur().right) {
            actor.direction = ActorDirection::Right;
            float accTime = colInfo.cur().below ? physic.accTimeGrounded : physic.accTimeAirborne;
            lerpVelocityX(physic, physic.moveSpeed, accTime, deltaTime);

        } else if(input.cur().left) {
            actor.direction = ActorDirection::Left;
            float accTime = colInfo.cur().below ? physic.accTimeGrounded : physic.accTimeAirborne;
            lerpVelocityX(physic, -physic.moveSpeed, accTime, deltaTime);
        }
    }
}

void ActorSystem::stateLadder(InputComponent& input, PhysicComponent& physic, CollisionInfoComponent& colInfo, ActorComponent& actor, SpriteComponent& sprite, float deltaTime) {

    // leave state if we're no longer on a ladder
    if(!colInfo.cur().onLadder && !colInfo.cur().onTopLadder) {
        if(colInfo.cur().below) {
            if(input.cur().left == input.cur().right) {
                sprite.setAnimIfNotCur("idle");
                actor.state = ActorState::Idle;
            } else {
                sprite.setAnimIfNotCur("walk");
                actor.state = ActorState::Move;
            }
        } else {
            if(input.cur().up) {
                physic.velocity.y = 0;
                //physic.velocity.y = colInfo.cur().distanceGround;
            }
            if(colInfo.cur().distanceGround > minFallDistance) {
                sprite.setAnimIfNotCur("descend");
            }
            actor.state = ActorState::Jump;
        }
        return;
    }

    bool vertIdle = false;
    bool horizIdle = false;

    // input
    if(input.cur().left == input.cur().right) {
        horizIdle = true;
        if(physic.velocity.x != 0) {
            physic.velocity.x = smoothDamp(physic.velocity.x, 0, physic.velocityXSmoothing, physic.accTimeGrounded, INFINITY,
                                           deltaTime);
        }
    } else if(input.cur().right) {
        sprite.setAnimIfNotCur("climb");
        sprite.resume();
        actor.direction = ActorDirection::Right;
        lerpVelocityX(physic, physic.ladderHorizSpeed, physic.accTimeGrounded, deltaTime);
    } else if(input.cur().left) {
        sprite.setAnimIfNotCur("climb");
        sprite.resume();
        actor.direction = ActorDirection::Left;
        lerpVelocityX(physic, -physic.ladderHorizSpeed, physic.accTimeGrounded, deltaTime);
    }

    if(input.cur().up == input.cur().down) {
        vertIdle = true;
        if(physic.velocity.y != 0) {
            lerpVelocityY(physic, 0, physic.accTimeGrounded, deltaTime);
        }
    } else if(input.cur().up) {
        sprite.setAnimIfNotCur("climb");
        sprite.resume();
        lerpVelocityY(physic, -physic.ladderVertSpeed, physic.accTimeGrounded, deltaTime);

        // if no sideways movement interpolate towards center of ladder
        if(input.cur().left == input.cur().right) {
            if(colInfo.cur().distanceLadderCenter != 0 ) {
                lerpVelocityX(physic, -colInfo.cur().distanceLadderCenter, physic.accTimeLadderCenter, deltaTime);
            }
        }
    } else if(input.cur().down) {
        sprite.setAnimIfNotCur("climb");
        sprite.resume();
        lerpVelocityY(physic, physic.ladderVertSpeed, physic.accTimeGrounded, deltaTime);

        // if no sideways movement interpolate towards center of ladder
        if(input.cur().left == input.cur().right) {
            if(colInfo.cur().distanceLadderCenter != 0 ) {
                lerpVelocityX(physic, -colInfo.cur().distanceLadderCenter, physic.accTimeLadderCenter, deltaTime);
            }
        }

        if(colInfo.cur().below && !colInfo.cur().onTopLadder) {
            SDL_Log("hit bottom of ladder");
            if(input.cur().left == input.cur().right) {
                sprite.setAnimIfNotCur("idle");
                actor.state = ActorState::Idle;
            } else {
                sprite.setAnimIfNotCur("walk");
                actor.state = ActorState::Move;
            }
        }
    }

    if(horizIdle && vertIdle) {
        sprite.setAnimIfNotCur("climb");
        sprite.pause();
    }
}

void ActorSystem::lerpVelocityX(PhysicComponent& physic, float target, float accTime,float deltaTime) {
    physic.velocity.x = smoothDamp(physic.velocity.x, target,
                                   physic.velocityXSmoothing, accTime,
                                   INFINITY,
                                   deltaTime);

}

void ActorSystem::lerpVelocityY(PhysicComponent& physic, float target, float accTime,float deltaTime) {
    physic.velocity.y = smoothDamp(physic.velocity.y, target,
                                   physic.velocityYSmoothing, accTime,
                                   INFINITY,
                                   deltaTime);

}

/*
void ActorSystem::update(Entity *ent, float deltaTime) {
    auto transform = ent->get<TransformComponent>();
    auto physic = ent->get<PhysicComponent>();
    auto sensor = ent->get<SensorComponent>();
    auto actor = ent->get<ActorComponent>();
    auto sprite = ent->get<SpriteComponent>();
    auto input = ent->get<InputComponent>();

    // only process entities with correct components
    if(!physic.isValid() || !transform.isValid() || !actor.isValid() || !sensor.isValid() || !sprite.isValid() || !input.isValid()) {
        return;
    }

    if(actor->direction == ActorDirection::Right) {
        sprite->horizFlip = false;
    }
    if(actor->direction == ActorDirection::Left) {
        sprite->horizFlip = true;
    }

    switch(actor->state) {
        case ActorState::Idle: {
            if(!sensor->cur().onGround) {
                actor->state = ActorState::Jump;
                sprite->setAnimIfNotCur("descend");
                break;
            }

            if(input->cur().left != input->cur().right) {
                if(input->cur().right)
                    actor->direction = ActorDirection::Right;
                if(input->cur().left)
                    actor->direction = ActorDirection::Left;
                actor->state = ActorState::Move;
                sprite->setAnimIfNotCur("walk");
                break;
            } else if(input->cur().jump) {
                startJump(actor.get(), physic.get(), sprite.get(), deltaTime);
                break;
            }
            break;
        }
        case ActorState::Move: {
            if (input->cur().left == input->cur().right) {
                actor->state = ActorState::Idle;
                sprite->setAnimIfNotCur("idle");
            } else if (input->cur().right) {
                if(!sensor->cur().pushesRightWall) {
                    physic->acceleration.set(actor->moveAcc, 0);
                } else {
                    physic->acceleration.x = 0;
                }
                actor->direction = ActorDirection::Right;
            } else if (input->cur().left) {
                if(!sensor->cur().pushesLeftWall) {
                    physic->acceleration.set(-actor->moveAcc, 0);
                } else {
                    physic->acceleration.x = 0;
                }
                actor->direction = ActorDirection::Left;
            }
            if(input->cur().jump) {
                startJump(actor.get(), physic.get(), sprite.get(), deltaTime);
            } else if(!sensor->cur().onGround) {
                actor->state = ActorState::Jump;
                sprite->setAnimIfNotCur("descend");
                break;
            }
            break;
        }
        case ActorState::Jump: {
            if(sensor->prev().ascending && !sensor->cur().ascending) {
                // reach jump curve apex
                sprite->setAnimIfNotCur("descend");
            }


            if (input->cur().left == input->cur().right) {
                physic->acceleration.x = 0.0f;
            } else if (input->cur().right) {
                if(!sensor->cur().pushesRightWall) {
                    physic->acceleration.set(actor->moveAcc, 0);
                } else {
                    physic->acceleration.x = 0;
                }
                actor->direction = ActorDirection::Right;
            } else if (input->cur().left) {
                if(!sensor->cur().pushesLeftWall) {
                    physic->acceleration.set(-actor->moveAcc, 0);
                } else {
                    physic->acceleration.x = 0;
                }
                actor->direction = ActorDirection::Left;
            }

            //if we hit the ground
            if (sensor->cur().onGround) {
                //if there's no movement change state to idle
                if (input->cur().left == input->cur().right)
                {
                    actor->state = ActorState::Idle;
                    sprite->setAnimIfNotCur("idle");
                }
                else    //either go right or go left are pressed so we change the state to walk
                {
                    actor->state = ActorState::Move;
                    sprite->setAnimIfNotCur("walk");
                }
            }
            break;
        }
        case ActorState::Punch:break;
        case ActorState::Kick:break;
        case ActorState::Shoot:break;
        case ActorState::Hurt:break;
        case ActorState::Dead:break;
        case ActorState::Ladder:break;
        case ActorState::Prune:break;
    }
}
*/

