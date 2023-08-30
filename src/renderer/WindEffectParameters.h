//
// Created by bison on 30-08-23.
//

#ifndef PLATFORMER_WINDEFFECTPARAMETERS_H
#define PLATFORMER_WINDEFFECTPARAMETERS_H

namespace Renderer {
    struct WindEffectParameters {
        float speed = 1.0f;
        float minStrength = 0.1f;
        float maxStrength = 0.5f;
        float strengthScale = 100.0;
        float interval = 3.5;
        float detail = 1.0;
        float distortion = 0; // hint_range(0.0, 1.0);
        float heightOffset = 0.0;
    };
}
#endif //PLATFORMER_WINDEFFECTPARAMETERS_H
