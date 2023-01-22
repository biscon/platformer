//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include <SDL_log.h>
#include <SDL_timer.h>
#include "Game.h"
#include "../renderer/Font.h"
#include "../util/string_util.h"
#include "PolyUtil.h"

using namespace Renderer;

Game::Game(Input::IInputDevice& inputDevice, IRenderDevice& renderDevice, RenderBuffers renderBuffers):
        inputDevice(inputDevice), renderDevice(renderDevice), renderBuffers(renderBuffers) {
    setupInputMapping();
    font = std::make_unique<Renderer::Font>(renderDevice, "assets/fonts/OpenSans-Semibold.ttf", 20);
    debugFont = std::make_unique<Renderer::Font>(renderDevice, "assets/fonts/ProggyClean.ttf", 16);
    inputContext = std::make_shared<Input::InputContext>();
    inputDevice.registerContext(inputContext);
    inputContext->registerAction(INPUT_ACTION_TOGGLE_FPS);
    inputContext->registerAction(INPUT_ACTION_TOGGLE_EDIT);
    inputContext->registerAction(INPUT_ACTION_ESCAPE);
    inputContext->registerAction(INPUT_ACTION_SAVE);
    inputContext->registerAction(INPUT_ACTION_LOAD);
    inputContext->registerState(INPUT_STATE_FORWARD);
    inputContext->registerState(INPUT_STATE_BACK);
    inputContext->registerState(INPUT_STATE_LEFT);
    inputContext->registerState(INPUT_STATE_RIGHT);

    level = std::make_shared<Level>(renderDevice, renderBuffers, inputDevice, *debugFont);

    editor = std::make_unique<Editor>(inputDevice, level->getWorld(), level->getCamera(), renderBuffers, *font);

    level->createTestLevel();

    //level->setDebug(true);
}

Game::~Game() {

}

bool Game::update(double frameDelta) {
    Input::Action action = {};
    renderBuffers.lit.pushClear(TRANSPARENT);
    renderBuffers.unlit.pushClear(TRANSPARENT);
    
    while(inputContext->pollAction(action)) {
        if(action.id == INPUT_ACTION_TOGGLE_FPS) {
            showFps = !showFps;
            level->setDebug(showFps);
        }
        if(action.id == INPUT_ACTION_TOGGLE_EDIT) {
            editMode = !editMode;
            level->setEditMode(editMode);
        }
        if(action.id == INPUT_ACTION_ESCAPE) {
            SDL_Log("Shutting down");
            return true;
        }
        if(action.id == INPUT_ACTION_SAVE) {
            level->save("desert_level.json");
        }

        if(action.id == INPUT_ACTION_LOAD) {
            level->transitionToLevel("desert_level.json", [this](){
                editor->reset();
            });
        }
    }

    //SDL_Log("New frame ----------------------------");
    level->update(static_cast<float>(frameDelta));

    if(editMode) {
        processEditModeInput(static_cast<float>(frameDelta));
        editor->update(static_cast<float>(frameDelta));
    }

    if(showFps) {
        double fps = 1.0 / frameDelta;
        float camX = level->getCamera().scrollX;
        float camY = level->getCamera().scrollY;
        renderBuffers.unlit.pushText(string_format("FPS %.2f  Cam: %.2f,%.2f %s", fps, camX, camY, editMode ? "EDIT" : ""), font.get(), 4, font->getSize() + 4, Renderer::WHITE);
        //SDL_Delay(33);
    }
    return false;
}

void Game::processEditModeInput(float deltaTime) {
    bool up = inputContext->queryState(INPUT_STATE_FORWARD);
    bool down = inputContext->queryState(INPUT_STATE_BACK);
    bool left = inputContext->queryState(INPUT_STATE_LEFT);
    bool right = inputContext->queryState(INPUT_STATE_RIGHT);
    float speed = 1200 * deltaTime;
    Vector2 move;
    if(left != right) {
        if(left) {
            move.x = -speed;
        }
        if(right) {
            move.x = speed;
        }
    }
    if(up != down) {
        if (up) {
            move.y = -speed;
        }
        if (down) {
            move.y = speed;
        }
    }
    level->translateEditCam(move);
}

void Game::setupInputMapping() {
    // Actions -------------------------------------------------------------------------------------
    Input::Mapping mapping{};
    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_ENTER;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_RETURN;
    inputDevice.createMapping(mapping);
    

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_ESCAPE;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_ESCAPE;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_SPACE;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_SPACE;
    inputDevice.createMapping(mapping);


    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOGGLE_FPS;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_F1;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOGGLE_FREECAM;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_F2;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOGGLE_EDIT;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_F3;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_LEFT;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_LEFT;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_RIGHT;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_RIGHT;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_UP;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_UP;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_DOWN;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_DOWN;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_SELECT_WEAPON1;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_1;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_SELECT_WEAPON2;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_2;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_DEL;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_DELETE;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_INS;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_INSERT;
    inputDevice.createMapping(mapping);

    // Tool shortcuts
    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOOL_1;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_1;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOOL_2;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_2;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOOL_3;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_3;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOOL_4;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_4;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOOL_5;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_5;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_TOOL_6;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_6;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_DUPLICATE;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_c;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_HIDE;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_h;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_GRID;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_g;
    inputDevice.createMapping(mapping);

    // save and load
    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_SAVE;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_F5;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_LOAD;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_F9;
    inputDevice.createMapping(mapping);

    // mouse buttons

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_LMB_DOWN;
    mapping.event.type = Input::RawEventType::Mouse;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_LMB_UP;
    mapping.event.type = Input::RawEventType::Mouse;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_RMB_DOWN;
    mapping.event.type = Input::RawEventType::Mouse;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::Action;
    mapping.mappedId = INPUT_ACTION_RMB_UP;
    mapping.event.type = Input::RawEventType::Mouse;
    inputDevice.createMapping(mapping);


    // States --------------------------------------------------------------------------------------

    // Arrow keys
    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_PUNCH;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_UP;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_KICK;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_DOWN;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_JUMP;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_SPACE;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_FORWARD;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_w;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_BACK;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_s;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_LEFT;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_a;
    inputDevice.createMapping(mapping);

    mapping.type = Input::MappingType::State;
    mapping.mappedId = INPUT_STATE_RIGHT;
    mapping.event.type = Input::RawEventType::Keyboard;
    mapping.event.keycode = SDLK_d;
    inputDevice.createMapping(mapping);

}
