#include <utility>

//
// Created by bison on 24-11-22.
//

#include "Level.h"
#include "systems/RenderLevelSystem.h"
#include "systems/UpdateAnimationSystem.h"
#include "Utils.h"
#include "systems/InputSystem.h"
#include "components/TransformComponent.h"
#include "components/TerrainComponent.h"
#include "components/ImageComponent.h"
#include "components/VerletMeshComponent.h"
#include "components/LadderComponent.h"
#include "components/PathComponent.h"
#include "components/FlickerEffectComponent.h"
#include "components/GlowEffectComponent.h"
#include "components/WindEffectComponent.h"

#include <json.hpp>
#include <iostream>
#include <fstream>
#include <SDL_log.h>

using json = nlohmann::json;


Level::Level(IRenderDevice &renderDevice, RenderBuffers renderBuffers, IInputDevice &inputDevice,
             Font &debugFont) : renderDevice(renderDevice), renderBuffers(renderBuffers),
                                inputDevice(inputDevice),
                                debugFont(debugFont) {

    animManager = std::make_unique<AnimationManager>(renderDevice);

    camera = std::make_unique<Camera>(config.width, config.height);
    transition = std::make_unique<TransitionEffect>(renderBuffers, config);

    scroller = std::make_unique<ParallaxScroller>(renderDevice);

    world = World::createWorld();
    world->registerSystem(new InputSystem(inputDevice));
    actorSystem = new ActorSystem();
    world->registerSystem(actorSystem);

    verletSystem = new VerletSystem();
    world->registerSystem(verletSystem);

    movingPlatformSystem = new PlatformSystem(renderBuffers.lit);
    world->registerSystem(movingPlatformSystem);

    collisionSystem = new CollisionSystem(renderBuffers.unlit);
    world->registerSystem(collisionSystem);

    world->registerSystem(new UpdateAnimationSystem());

    updateEffectsSystem = new UpdateEffectsSystem();
    world->registerSystem(updateEffectsSystem);

    renderLevelSystem = new RenderLevelSystem(renderBuffers, *camera, *scroller, config);
    world->registerSystem(renderLevelSystem);
    renderDebugSystem = new RenderDebugSystem(renderBuffers.unlit, debugFont, *camera);
    world->registerSystem(renderDebugSystem);


    clear();

    //renderDevice.enableCrtFx(true);
    /*
    config.colorGrading.lut1Texture = renderDevice.uploadPNGTexture("assets/lut.png", true);
    config.colorGrading.lut2Texture = renderDevice.uploadPNGTexture("assets/lut/Late_Night.png", true);
    renderDevice.setLut1Texture(config.colorGrading.lut1Texture);
    renderDevice.setLut2Texture(config.colorGrading.lut2Texture);
    renderDevice.setLutMix(1.0f);
    renderDevice.enableLut(true);
    */

}

Level::~Level() {
    freeLutTextures();
    world->destroyWorld();
    world = nullptr;
}

void Level::freeLutTextures() {
    if(config.colorGrading.lut1Texture > 0) {
        renderDevice.deleteTexture(&config.colorGrading.lut1Texture);
        config.colorGrading.lut1Texture = 0;
    }
    if(config.colorGrading.lut2Texture > 0) {
        renderDevice.deleteTexture(&config.colorGrading.lut2Texture);
        config.colorGrading.lut2Texture = 0;
    }
}

void Level::createTestLevel() {
    config.skyColor = {33.0f/255.0f, 109.0f/255.0f, 154.0f/255.0f, 0.0f};

    // build terrain
    auto ground = world->create();
    ground->assign<TerrainComponent>(std::vector<Vector2> {{0, 800}, {300, 800}, {500, 900}, {700, 900}, {1000, 800}, {1200, 600}, {1500, 600}, {1500, 500}, {1920, 500}, {2500, 1000}, {2500, 1050}, {1500, 1050}, {1500, 1300}, {2500, 1300}, {3840, 2000}, {3840 ,2160}, {0, 2160}}, true);
    ground->assign<TransformComponent>(Vector2(0, 0), 1.0f, 0, 0);

    auto platform1 = world->create();
    platform1->assign<TerrainComponent>(std::vector<Vector2> {{300,500},{500,500},{500,520},{300,520}}, true);
    platform1->assign<TransformComponent>(Vector2(0, 0), 1.0f, 0, 0);

    auto platform2 = world->create();
    platform2->assign<TerrainComponent>(std::vector<Vector2> {{600,500},{800,500},{800,520},{600,520}}, true);
    platform2->assign<TransformComponent>(Vector2(0, 0), 1.0f, 0, 0);

    auto ladder1 = world->create();
    ladder1->assign<LadderComponent>(FloatRect(500,500,600,900));

    auto atlas = std::make_shared<TextureAtlas>(renderDevice, 1024, 1024, PixelFormat::RGBA);

    auto image1 = world->create();
    image1->assign<TransformComponent>(Vector2(500, 300), 4.0f, 0, -1);
    image1->assign<ImageComponent>("assets/objects/bus_stop.png", renderDevice);

    auto image2 = world->create();
    image2->assign<TransformComponent>(Vector2(900, 300), 4.0f, 0, 1);
    image2->assign<ImageComponent>("assets/lights/pendel_4x.png", renderDevice);
    image2->assign<VerletMeshComponent>();
    image2->assign<GlowEffectComponent>();
    image2->get<VerletMeshComponent>()->buildRope(Vector2(1000,100));

    auto movingPlatform1 = world->create();
    movingPlatform1->assign<TransformComponent>(Vector2(100, 300), 1.0f, 0, 0);
    movingPlatform1->assign<TerrainComponent>();
    movingPlatform1->assign<PlatformComponent>(Vector2(100, 50), 200.0f);
    movingPlatform1->assign<CollisionComponent>();
    //movingPlatform1->assign<VerletMeshComponent>();
    //movingPlatform1->get<VerletMeshComponent>()->buildRope(Vector2(700,0));
    //movingPlatform1->assign<PathComponent>(std::vector<Vector2> {{100, 300}, {600, 100}, {1000, 300}, {2000, 0}, {3700, 2000}}, true);

    /*
    auto verletRope = world->create();
    verletRope->assign<TransformComponent>(Vector2(1000, 100), 1.0f, 0, 0);
    verletRope->assign<VerletMeshComponent>();
    verletRope->get<VerletMeshComponent>()->buildRope(Vector2(1000,100));
     */

    // build parallax scroller
    /*
    scroller->createLayerFromPNG("assets/backgrounds/mountains/back.png", 1920, 1080, 1080, 0.01f, false);
    scroller->createLayerFromPNG("assets/backgrounds/mountains/shadow.png", 1920, 1080, 1080, 0.25f, false);
    scroller->createLayerFromPNG("assets/backgrounds/mountains/back2.png", 1920, 1080, 1080, 0.5f, false);
    //scroller->createLayerFromPNG("assets/backgrounds/mountains/mid.png", 1920, 1080, 1080, 1.0f, false);
    scroller->uploadTextures();
     */

    atlas->packAndUpload(renderDevice);

    createPlayer();
}

void Level::createPlayer() {
    FloatRect noInsets = { 0, 0, 0, 0 };
    FloatRect widthInsets(0.25f, 0, 0.25f, 0);
    FloatRect jumpInsets(0.25f, 0.2f, 0.25f, 0);
    //Renderer::FloatRect attackInsets = { .left = 0.3333f, .right = 0.3333f, .top = .0, .bottom = 0 };

    auto atlas = std::make_shared<TextureAtlas>(renderDevice, 512, 512, PixelFormat::RGBA);
    player = world->create();
    player->setName("Player");
    Vector2 pos = Vector2(120.0f, 150.0f);

    if(!config.spawns.empty()) {
        pos = config.spawns[0].pos;
    }

    player->assign<TransformComponent>(pos, 5.0f, 0, 0);
    player->assign<InputComponent>();
    player->assign<CollisionInfoComponent>();
    player->assign<PhysicComponent>();
    player->assign<CollisionComponent>();
    player->assign<ActorComponent>("player");
    player->assign<SpriteComponent>();
    auto spr1 = player->get<SpriteComponent>();
    auto idle = Animation::createFromPNG("assets/sprites/player_idle.png", 32, 48, 16, 48, 4, atlas, &widthInsets);
    spr1->createAnimation("idle", RepeatType::Restart, idle);

    auto walk = Animation::createFromPNG("assets/sprites/player_walk.png", 32, 48, 16, 48, 8, atlas, &widthInsets);
    spr1->createAnimation("walk", RepeatType::Restart, walk);

    auto ascend = Animation::createFromPNG("assets/sprites/player_ascend.png", 32, 64, 16, 64, 8, atlas, &jumpInsets);
    spr1->createAnimation("ascend", RepeatType::Once, ascend);

    auto descend = Animation::createFromPNG("assets/sprites/player_descend.png", 32, 64, 16, 64, 2, atlas, &jumpInsets);
    spr1->createAnimation("descend", RepeatType::Once, descend);

    auto climb = Animation::createFromPNG("assets/sprites/player_climb.png", 32, 64, 16, 64, 8, atlas, &jumpInsets);
    spr1->createAnimation("climb", RepeatType::Restart, climb);

    spr1->setAnim("idle");

    atlas->packAndUpload(renderDevice);

    FloatRect bounds;
    bounds.zero();
    getBounds(player, bounds);
    camera->focus(bounds, Vector2(1920.0f/6.0f, 800.0f));
}

void Level::update(float deltaTime) {

    renderBuffers.background.pushClear(config.skyColor);
    renderBuffers.light.pushClear(config.ambientColor);

    //renderBuffers.lit.pushClear(TRANSPARENT);

    if(editMode) {
        camera->update(camTarget, deltaTime);
    } else {
        // update camera
        FloatRect bounds;
        bounds.zero();
        getBounds(player, bounds);
        auto input = player->get<InputComponent>();
        if(input.isValid()) {
            camera->update(bounds, deltaTime, input.get(), true);
        }
    }

    world->tick(deltaTime);

    if(loading) {
        if(transition->isDone && transition->state == TransitionState::Out) {
            load(filenameToLoad);
            FloatRect bounds;
            bounds.zero();
            getBounds(player, bounds);
            Vector2 focus = Vector2(bounds.centerX() - camera->scrollX, bounds.centerY() - camera->scrollY);
            transition->start(TransitionState::In, focus);
            if(onTransitionDone != nullptr) {
                onTransitionDone();
            }
        } else if(transition->isDone && transition->state == TransitionState::In) {
            loading = false;
        }
        transition->update(deltaTime);
        transition->render();
    }
}


void Level::translateEditCam(Vector2& offset) {
    camTarget += offset;
}

void Level::setDebug(bool debug) {
    this->debug = debug;
    if(renderDebugSystem) {
        renderDebugSystem->isActive = this->debug;
    }
}

void Level::setEditMode(bool editMode) {
    this->editMode = editMode;
    renderLevelSystem->editMode = editMode;
    if(editMode) {
        FloatRect bounds;
        bounds.zero();
        getBounds(player, bounds);

        camTarget = Vector2(bounds.left + (bounds.width()*0.5f),
                            bounds.top + (bounds.height()*0.5f));

        world->disableSystem(actorSystem);
        world->disableSystem(collisionSystem);
        world->disableSystem(movingPlatformSystem);
        world->disableSystem(verletSystem);
        world->disableSystem(updateEffectsSystem);
    } else {
        world->enableSystem(actorSystem);
        world->enableSystem(collisionSystem);
        world->enableSystem(movingPlatformSystem);
        world->enableSystem(verletSystem);
        world->enableSystem(updateEffectsSystem);
    }
}

void Level::clear() {
    animManager->clear();
    config.spawns.clear();
    camera->lockYAxis = false;
    //editor->reset();
    world->reset();
}

void Level::newLevel() {
    clear();
    config = LevelConfig();
    camera->setLevelDimensions(config.width, config.height);
    camera->lockYAxis = false;
    scroller->reset();

    // build terrain
    auto ground = world->create();
    ground->assign<TerrainComponent>(std::vector<Vector2> {{0, 900}, {1920, 900}, {1920, 1080}, {0, 1080}}, true);
    ground->assign<TransformComponent>(Vector2(0, 0), 1.0f, 0, 0);
    ground->setName("Ground");

    createPlayer();
}

void Level::save(std::string filename) {
    json j;
    // read in existing file first if any as to not discard any elements save can't handle

    if(levelFile) {
        std::ifstream file(levelFile->filename);
        file >> j;
    }

    j["title"] = filename;
    j["width"] = config.width;
    j["height"] = config.height;
    j["lockYAxis"] = camera->lockYAxis;
    j["lockedCamY"] = camera->lockedCamY;

    j["skyColor"].clear();
    pushColor(j["skyColor"], config.skyColor);
    j["ambientColor"].clear();
    pushColor(j["ambientColor"], config.ambientColor);

    if(!scroller->layers.empty()) {
        if(j.contains("scroller")) {
            if(j["scroller"].contains("layers"))
                j["scroller"]["layers"].clear();
        }

        json layers;
        for(auto& layer : scroller->layers) {
            json jsonLayer;
            jsonLayer["filename"] = layer.filename;
            jsonLayer["width"] = layer.width;
            jsonLayer["height"] = layer.height;
            jsonLayer["offsetY"] = layer.posY;
            jsonLayer["speed"] = layer.speedFac;
            jsonLayer["foreground"] = layer.foreground;
            layers.push_back(jsonLayer);
        }
        j["scroller"]["layers"] = layers;
    }

    j["animations"].clear();
    animManager->save(j["animations"]);

    j["entities"].clear();
    for (Entity* ent : world->all(false)) {
        if(ent == player) {
            continue;
        }
        json jsonEnt;
        jsonEnt["name"] = ent->getName();
        auto transform = ent->get<TransformComponent>();
        auto terrain = ent->get<TerrainComponent>();
        auto ladder = ent->get<LadderComponent>();
        auto image = ent->get<ImageComponent>();
        auto platform = ent->get<PlatformComponent>();
        auto path = ent->get<PathComponent>();
        auto collision = ent->get<CollisionComponent>();
        auto light = ent->get<PointLightComponent>();
        auto sprite = ent->get<SpriteComponent>();
        auto verletMesh = ent->get<VerletMeshComponent>();
        auto flickerEffect = ent->get<FlickerEffectComponent>();
        auto glowEffect = ent->get<GlowEffectComponent>();
        auto windEffect = ent->get<WindEffectComponent>();

        if(transform.isValid()) transform->save(jsonEnt);
        if(terrain.isValid()) terrain->save(jsonEnt);
        if(ladder.isValid()) ladder->save(jsonEnt);
        if(image.isValid()) image->save(jsonEnt);
        if(platform.isValid()) platform->save(jsonEnt);
        if(path.isValid()) path->save(jsonEnt);
        if(collision.isValid()) collision->save(jsonEnt);
        if(light.isValid()) light->save(jsonEnt);
        if(verletMesh.isValid()) verletMesh->save(jsonEnt);
        if(flickerEffect.isValid()) flickerEffect->save(jsonEnt);
        if(glowEffect.isValid()) glowEffect->save(jsonEnt);
        if(sprite.isValid()) sprite->save(jsonEnt);
        if(windEffect.isValid()) windEffect->save(jsonEnt);

        j["entities"].push_back(jsonEnt);
    }

    j["spawns"].clear();
    for(auto& spawn : config.spawns) {
        json s;
        s["id"] = spawn.id;
        pushPos(s["pos"], spawn.pos);
        j["spawns"].push_back(s);
    }

    // color grading
    j["colorGrading"].clear();
    json cgJson;
    cgJson["lut1Filename"] = config.colorGrading.lut1Filename;
    cgJson["lut2Filename"] = config.colorGrading.lut2Filename;
    cgJson["mix"] = config.colorGrading.mix;
    cgJson["applyBackground"] = config.colorGrading.applyBackground;
    j["colorGrading"] = cgJson;

    std::cout << std::setw(4) << j << std::endl;
    std::ofstream file(filename);
    file << std::setw(4) << j << std::endl;
}

void Level::load(std::string filename) {
    std::ifstream file(filename);
    json j;
    file >> j;
    clear();

    config.filename = filename;
    config.width = j["width"];
    config.height = j["height"];
    camera->setLevelDimensions(config.width, config.height);
    if(j.contains("lockYAxis")) {
        camera->lockYAxis = j["lockYAxis"];
    }
    if(j.contains("lockedCamY")) {
        camera->lockedCamY = j["lockedCamY"];
    }

    scroller->reset();
    if (j.contains("scroller")) {
        for (auto &l : j["scroller"]["layers"]) {
            scroller->createLayerFromPNG(l["filename"], l["width"], l["height"], l["offsetY"],
                                         l["speed"], l["foreground"]);
        }
        scroller->uploadTextures();
    }

    if (j.contains("skyColor")) {
        config.skyColor = {.r = j["skyColor"][0], .g = j["skyColor"][1], .b = j["skyColor"][2], .a = j["skyColor"][3]};
    }

    if (j.contains("ambientColor")) {
        config.ambientColor = {.r = j["ambientColor"][0], .g = j["ambientColor"][1], .b = j["ambientColor"][2], .a = j["ambientColor"][3]};
    }

    if(j.contains("animations")) {
        animManager->load(j["animations"]);
    }

    std::unordered_map<std::string, ImageComponent*> imagesCache;

    for(auto& e : j["entities"]) {
        auto ent = world->create();
        if(e.contains("name")) {
            ent->setName(e["name"]);
        }
        if(e.contains("transform")) {
            ent->assign<TransformComponent>(e["transform"]);
        }
        if(e.contains("terrain")) {
            ent->assign<TerrainComponent>(e["terrain"]);
        }
        if(e.contains("ladder")) {
            ent->assign<LadderComponent>(e["ladder"]);
        }
        if(e.contains("image")) {
            auto fn = e["image"]["filename"];
            if(imagesCache.count(fn) > 0) {
                ent->assign<ImageComponent>(*imagesCache[fn]);
            } else {
                ent->assign<ImageComponent>(fn, renderDevice);
                auto image = ent->get<ImageComponent>();
                imagesCache[image->filename] = &image.get();
            }
            ent->get<ImageComponent>()->load(e["image"]);

        }
        if(e.contains("platform")) {
            ent->assign<PlatformComponent>(e["platform"]);
            rebuildMovingPlatform(ent);
        }
        if(e.contains("path")) {
            ent->assign<PathComponent>(e["path"]);
        }
        if(e.contains("collision")) {
            ent->assign<CollisionComponent>();
        }
        if(e.contains("pointLight")) {
            ent->assign<PointLightComponent>(e["pointLight"]);
        }
        if(e.contains("sprite")) {
            ent->assign<SpriteComponent>(e["sprite"], *animManager);
        }
        if(e.contains("verletMesh")) {
            ent->assign<VerletMeshComponent>(e["verletMesh"]);
        }
        if(e.contains("flickerEffect")) {
            ent->assign<FlickerEffectComponent>(e["flickerEffect"]);
        }
        if(e.contains("glowEffect")) {
            ent->assign<GlowEffectComponent>(e["glowEffect"]);
        }
        if(e.contains("windEffect")) {
            ent->assign<WindEffectComponent>(e["windEffect"]);
        }
    }

    for(auto& e : j["spawns"]) {
        Spawn spawn = { e["id"], Vector2(e["pos"][0], e["pos"][1]) };
        config.spawns.emplace_back(spawn);
    }

    if(j.contains("colorGrading")) {
        json cgJson = j["colorGrading"];
        config.colorGrading.lut1Filename = cgJson["lut1Filename"];
        config.colorGrading.lut2Filename = cgJson["lut2Filename"];
        config.colorGrading.mix = cgJson["mix"];
        config.colorGrading.applyBackground = cgJson["applyBackground"];
        if(config.colorGrading.applyBackground) {
            setupColorGrading();
        }
    } else {
        renderDevice.enableLut(false);
        freeLutTextures();
    }

    levelFile = std::make_unique<LevelFile>(filename);
    SDL_Log("Uploaded %d images", (i32) imagesCache.size());

    createPlayer();
    animManager->upload();
}

void Level::transitionToLevel(std::string filename, std::function<void()> callback) {
    onTransitionDone = std::move(callback);
    filenameToLoad = std::move(filename);
    loading = true;
    FloatRect bounds;
    bounds.zero();
    getBounds(player, bounds);
    Vector2 focus = Vector2(bounds.centerX(), bounds.centerY());
    focus -= Vector2(camera->scrollX, camera->scrollY);
    transition->start(TransitionState::Out, focus);
    //transition->start(TransitionState::In, Vector2(1920/2, 1080/2));
}

void Level::setupColorGrading() {
    freeLutTextures();
    config.colorGrading.lut1Texture = renderDevice.uploadPNGTexture(config.colorGrading.lut1Filename, true);
    renderDevice.setLut1Texture(config.colorGrading.lut1Texture);
    config.colorGrading.lut2Texture = renderDevice.uploadPNGTexture(config.colorGrading.lut2Filename, true);
    renderDevice.setLut2Texture(config.colorGrading.lut2Texture);
    renderDevice.setLutMix(config.colorGrading.mix);
    renderDevice.enableLut(true);
}