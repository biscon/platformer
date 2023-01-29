//
// Created by bison on 29-01-23.
//

#ifndef PLATFORMER_IMAGEPROPERTYEDITOR_H
#define PLATFORMER_IMAGEPROPERTYEDITOR_H

#include "IComponentPropertyEditor.h"
#include "../../input/Input.h"



class ImagePropertyEditor: public IComponentPropertyEditor {
public:
    void show() override;
    void setSelected(Entity *ent) override;
    bool onLeftDown(Vector2 pos) override;
    bool onLeftUp(Vector2 pos) override;
    bool onAction(const Input::Action &action) override;

private:
    Entity* selected = nullptr;
};


#endif //PLATFORMER_IMAGEPROPERTYEDITOR_H
