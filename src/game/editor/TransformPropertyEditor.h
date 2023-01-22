//
// Created by bison on 22-01-23.
//

#ifndef PLATFORMER_TRANSFORMPROPERTYEDITOR_H
#define PLATFORMER_TRANSFORMPROPERTYEDITOR_H


#include "IComponentPropertyEditor.h"

class TransformPropertyEditor: public IComponentPropertyEditor {
public:
    void show() override;
    void setSelected(Entity *ent) override;

private:
    Entity* selected = nullptr;
};


#endif //PLATFORMER_TRANSFORMPROPERTYEDITOR_H
