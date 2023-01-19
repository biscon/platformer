#include <utility>

//
// Created by bison on 18-01-23.
//

#include "Editor.h"

Editor::Editor(std::shared_ptr<Level> level, IInputDevice &inputDevice, RenderBuffers renderBuffers,
               Font &font) : level(std::move(level)), inputDevice(inputDevice), renderBuffers(renderBuffers), font(font) {

}