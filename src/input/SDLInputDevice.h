//
// Created by bison on 10-11-22.
//

#ifndef GAME_SDLINPUTDEVICE_H
#define GAME_SDLINPUTDEVICE_H

#include "Input.h"

namespace Input {
    class SDLInputDevice : public IInputDevice {
    public:
        SDLInputDevice();

        void createMapping(Mapping& mapping) override;
        void registerContext(std::shared_ptr<InputContext>& context) override;
        void showMouseCursor(bool show) override;
        const MouseState& getMouseState() override;

        void onPushKeyUp(SDL_Keycode keycode);
        void onPushKeyDown(SDL_Keycode keycode);
        void onMouseMotion(float x, float y);
        void onMouseLeftButton(bool pressed);
        void onMouseRightButton(bool pressed);


    private:
        std::vector<std::shared_ptr<InputContext>> contexts;
        std::vector<Mapping> mappings;
        MouseState mouseState;

        State* findOrCreateState(InputContext& context, Mapping &mapping);
        void dispatchActionMapping(Mapping& mapping);
        void dispatchStateMapping(Mapping& mapping, bool active);
    };
}


#endif //GAME_SDLINPUTDEVICE_H
