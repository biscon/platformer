//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_INPUTCOMPONENT_H
#define PLATFORMER_INPUTCOMPONENT_H


struct Inputs {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool jump = false;
    bool firstAttack = false;
    bool secondAttack = false;
    float mouseX = false;
    float mouseY = false;
};

struct InputComponent {
    Inputs inputs[2];
    Inputs& cur() { return inputs[0]; }
    Inputs& prev() { return inputs[1]; }
};

#endif //PLATFORMER_INPUTCOMPONENT_H
