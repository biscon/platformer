//
// Created by bison on 22-01-23.
//

#ifndef PLATFORMER_ICOMPONENTPROPERTYEDITOR_H
#define PLATFORMER_ICOMPONENTPROPERTYEDITOR_H

#include <Vector2.h>
#include <ECS.h>

using namespace ECS;

class IComponentPropertyEditor {
public:
    virtual ~IComponentPropertyEditor() = default;
    virtual void show() = 0;
    virtual void setSelected(Entity* ent) = 0;
};


#endif //PLATFORMER_ICOMPONENTPROPERTYEDITOR_H
