//
// Created by bison on 21-12-22.
//

#ifndef PLATFORMER_ITOOL_H
#define PLATFORMER_ITOOL_H

#include <Vector2.h>
#include "../../input/Input.h"
#include "../../renderer/Types.h"

using namespace ECS;
using namespace Input;

class ITool {
public:
    virtual ~ITool() = default;
    virtual void onUpdate() = 0;
    virtual bool onKeyboard(MappedId key) = 0;
    virtual bool onLeftDown(Vector2 pos) = 0;
    virtual bool onLeftUp(Vector2 pos) = 0;
    virtual void reset() = 0;
    virtual void setSelected(Entity* ent) = 0;
    virtual Entity* getNewSelected() = 0;
};

#endif //PLATFORMER_ITOOL_H
