#include <utility>

//
// Created by bison on 24-11-22.
//

#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <memory>
#include <ECS.h>
#include <RenderBuffers.h>
#include <LevelConfig.h>
#include "../renderer/RenderCmdBuffer.h"
#include "../renderer/Font.h"
#include "../renderer/Gui.h"
#include "../input/Input.h"
#include "systems/RenderDebugSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/ActorSystem.h"
#include "editor/Editor.h"
#include "systems/RenderLevelSystem.h"
#include "Camera.h"
#include "ParallaxScroller.h"
#include "systems/PlatformSystem.h"
#include "systems/VerletSystem.h"
#include "systems/UpdateEffectsSystem.h"
#include "TransitionEffect.h"

using namespace Renderer;
using namespace Input;

struct LevelFile {
    LevelFile(std::string filename) : filename(std::move(filename)) {}
    std::string filename;
};

class Level {
public:
    Level(IRenderDevice& renderDevice, RenderBuffers renderBuffers, IInputDevice &inputDevice, Font &debugFont);
    virtual ~Level();
    void createTestLevel();

    void update(float deltaTime);
    void setDebug(bool debug);
    void setEditMode(bool editMode);
    Camera& getCamera() { return *camera; }
    World* getWorld() { return world; }
    void translateEditCam(Vector2& offset);

    void clear();
    void save(std::string filename);
    void load(std::string filename);

    void transitionToLevel(std::string filename);

private:
    IRenderDevice& renderDevice;
    RenderBuffers renderBuffers;
    IInputDevice& inputDevice;
    bool debug = false;
    bool editMode = false;
    Font& debugFont;
    World* world = nullptr;
    Entity* player = nullptr;
    std::unique_ptr<Camera> camera;
    RenderDebugSystem* renderDebugSystem = nullptr;
    RenderLevelSystem* renderLevelSystem = nullptr;
    ActorSystem* actorSystem = nullptr;
    CollisionSystem* collisionSystem = nullptr;
    PlatformSystem* movingPlatformSystem = nullptr;
    VerletSystem* verletSystem = nullptr;
    UpdateEffectsSystem* updateEffectsSystem = nullptr;
    std::unique_ptr<LevelFile> levelFile;
    std::unique_ptr<ParallaxScroller> scroller;
    std::unique_ptr<TransitionEffect> transition;
    u32 lut1Texture = 0;
    u32 lut2Texture = 0;

    LevelConfig config;
    std::string filenameToLoad;
    bool loading = false;
    Vector2 camTarget;

    void freeLutTextures();
    void createPlayer();
};


#endif //GAME_LEVEL_H
