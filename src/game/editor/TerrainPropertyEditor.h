//
// Created by bison on 22-01-23.
//

#ifndef PLATFORMER_TERRAINPROPERTYEDITOR_H
#define PLATFORMER_TERRAINPROPERTYEDITOR_H


#include "IComponentPropertyEditor.h"

class TerrainPropertyEditor: public IComponentPropertyEditor {
public:
    void show() override;
    void setSelected(Entity *ent) override;

private:
    Entity* selected = nullptr;
};


#endif //PLATFORMER_TERRAINPROPERTYEDITOR_H
