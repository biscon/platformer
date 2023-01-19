#include <utility>

//
// Created by bison on 22-11-22.
//

#ifndef GAME_ACTORCOMPONENT_H
#define GAME_ACTORCOMPONENT_H

#include <string>

enum class ActorState {
    Idle,
    Move,
    Jump,
    Punch,
    Kick,
    Shoot,
    Hurt,
    Dead,
    Ladder,
    Prune
};

enum class ActorDirection {
    Left,
    Right
};

class ActorComponent {
public:
    std::string name;
    ActorState state = ActorState::Idle;
    ActorDirection direction = ActorDirection::Right;

    explicit ActorComponent(std::string name) : name(std::move(name)) {}
};


#endif //GAME_ACTORCOMPONENT_H
