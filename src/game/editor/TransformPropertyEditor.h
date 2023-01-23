//
// Created by bison on 22-01-23.
//

#ifndef PLATFORMER_TRANSFORMPROPERTYEDITOR_H
#define PLATFORMER_TRANSFORMPROPERTYEDITOR_H


#include "IComponentPropertyEditor.h"
#include "../../input/Input.h"

class TransformPropertyEditor: public IComponentPropertyEditor {
public:
    void show() override;
    void setSelected(Entity *ent) override;

    bool onLeftDown(Vector2 pos) override;
    bool onLeftUp(Vector2 pos) override;

    bool onAction(const Input::Action &action) override;

private:
    Entity* selected = nullptr;
};


#endif //PLATFORMER_TRANSFORMPROPERTYEDITOR_H
