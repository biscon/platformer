//
// Created by bison on 20-12-22.
//

#ifndef PLATFORMER_GUI_H
#define PLATFORMER_GUI_H

#include <memory>
#include "RenderCmdBuffer.h"
#include "../input/Input.h"

using namespace Input;

namespace Renderer {
    const Color UI_BUTTON = {0.5f, 0.5f, 0.5f, 1.0f};
    const Color UI_DARK = {0.25f, 0.25f, 0.25f, 1.0f};
    const Color UI_LIGHT = {0.85f, 0.85f, 0.85f, 1.0f};

    class IGuiElement {
    public:
        virtual ~IGuiElement() = default;
        virtual FloatRect &getBounds() = 0;
        virtual void render(RenderCmdBuffer& buffer, Font &font, float deltaTime) = 0;
        virtual void onClick() = 0;
    };

    class ToggleButton : public IGuiElement {
    public:
        FloatRect &getBounds() override;
        void render(RenderCmdBuffer &buffer, Font &font, float deltaTime) override;
        void onClick() override;

        FloatRect bounds;
        std::string text;
        bool toggled;
    };

    class Gui {
    public:
        Gui(RenderCmdBuffer &buffer, Font &font, IInputDevice &inputDevice);

        void processInput();
        void render(float deltaTime);

        void add(IGuiElement* element) {
            elements.push_back(element);
        }

        bool wasClicked(IGuiElement *element);

    private:
        RenderCmdBuffer& buffer;
        Font &font;
        IInputDevice& inputDevice;
        std::shared_ptr<Input::InputContext> inputContext;
        std::vector<IGuiElement*> elements;
        IGuiElement* lastClicked = nullptr;

        void dispatchClick();
    };
}

#endif //PLATFORMER_GUI_H
