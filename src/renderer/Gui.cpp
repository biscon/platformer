//
// Created by bison on 20-12-22.
//

#include <SDL_log.h>
#include "Gui.h"

namespace Renderer {

    Gui::Gui(RenderCmdBuffer &buffer, Font &font, IInputDevice &inputDevice) : buffer(buffer),
    font(font),
    inputDevice(inputDevice) {
        inputContext = std::make_shared<Input::InputContext>();
        inputDevice.registerContext(inputContext);
        inputContext->registerAction(INPUT_ACTION_LMB_DOWN);
        inputContext->registerAction(INPUT_ACTION_LMB_UP);
    }

    void Gui::processInput() {
        Input::Action action = {};

        while(inputContext->pollAction(action)) {
            if(action.id == INPUT_ACTION_LMB_DOWN) {
                dispatchClick();
            }
            if(action.id == INPUT_ACTION_LMB_UP) {

            }
        }
    }

    void Gui::render(float deltaTime) {
        for(IGuiElement* element : elements) {
            element->render(buffer, font, deltaTime);
        }
    }

    void Gui::dispatchClick() {
        auto &mouse = inputDevice.getMouseState();
        for(IGuiElement* element : elements) {
            if(element->getBounds().containsPoint(mouse.x, mouse.y)) {
                element->onClick();
                lastClicked = element;
                return;
            }
        }
    }

    bool Gui::wasClicked(IGuiElement *element) {
        if(lastClicked == element) {
            lastClicked = nullptr;
            return true;
        }
        return false;
    }

    FloatRect &ToggleButton::getBounds() {
        return bounds;
    }

    void ToggleButton::render(RenderCmdBuffer &buffer, Font &font, float deltaTime) {
        Quad q = { .color = BLACK };
        q.setBounds(bounds);
        buffer.pushQuad(q);

        FloatRect inset = bounds;
        inset.expand(-2);
        q.color = UI_BUTTON;
        q.setBounds(inset);
        buffer.pushQuad(q);

        auto topLeft = Vector2(inset.left, inset.top);
        auto topRight = Vector2(inset.right, inset.top);
        auto bottomLeft = Vector2(inset.left, inset.bottom);
        auto bottomRight = Vector2(inset.right, inset.bottom);
        buffer.pushLine(topLeft, topRight, toggled ? UI_DARK : UI_LIGHT);
        buffer.pushLine(topLeft, bottomLeft, toggled ? UI_DARK : UI_LIGHT);
        buffer.pushLine(topRight, bottomRight, toggled ? UI_LIGHT : UI_DARK);
        buffer.pushLine(bottomLeft, bottomRight, toggled ? UI_LIGHT : UI_DARK);
        Vector2 pos(inset.left + inset.width() * 0.5f, inset.top + inset.height()/2);
        pos.x -= font.measureTextWidth(text)/2.0f;
        pos.y -= font.getSize()/2.0f;
        buffer.pushText(text, &font, pos.x, pos.y + font.getSize() - 2, WHITE );
    }

    void ToggleButton::onClick() {

    }
}