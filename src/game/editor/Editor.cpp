//
// Created by bison on 20-12-22.
//

#include <SDL_log.h>
#include "Editor.h"
#include "../components/TerrainComponent.h"
#include "../PolyUtil.h"
#include "../../util/string_util.h"
#include "MoveTool.h"
#include "TerrainTool.h"
#include "RectangleTool.h"
#include "LadderTool.h"
#include "../components/ImageComponent.h"
#include "ImageTool.h"
#include "ToolUtil.h"
#include "PlatformTool.h"
#include "../components/CollisionComponent.h"
#include "../Utils.h"
#include "PathTool.h"
#include "LightTool.h"
#include "../components/PointLightComponent.h"
#include "../components/PlatformComponent.h"
#include "../components/TransformComponent.h"
#include "../components/PathComponent.h"
#include "../components/LadderComponent.h"
#include "../imeditor/Editor.h"


Editor::Editor(std::unique_ptr<Gui> &gui, IInputDevice &inputDevice, World* world, Camera& camera, RenderBuffers buffers, Font &font) :
        ui(std::move(gui)), inputDevice(inputDevice), world(world), camera(camera), buffers(buffers), font(font) {
    inputContext = std::make_shared<Input::InputContext>();
    inputDevice.registerContext(inputContext);
    inputContext->registerAction(INPUT_ACTION_LMB_DOWN);
    inputContext->registerAction(INPUT_ACTION_LMB_UP);
    inputContext->registerAction(INPUT_ACTION_DEL);
    inputContext->registerAction(INPUT_ACTION_INS);
    inputContext->registerAction(INPUT_ACTION_ENTER);
    inputContext->registerAction(INPUT_ACTION_DUPLICATE);
    inputContext->registerAction(INPUT_ACTION_HIDE);
    inputContext->registerAction(INPUT_ACTION_GRID);

    inputContext->registerAction(INPUT_ACTION_TOOL_1);
    inputContext->registerAction(INPUT_ACTION_TOOL_2);
    inputContext->registerAction(INPUT_ACTION_TOOL_3);
    inputContext->registerAction(INPUT_ACTION_TOOL_4);
    inputContext->registerAction(INPUT_ACTION_TOOL_5);
    inputContext->registerAction(INPUT_ACTION_TOOL_6);

    inputContext->registerAction(INPUT_ACTION_UP);
    inputContext->registerAction(INPUT_ACTION_DOWN);

    float margin = 4;
    float width = 150;
    float height = 50;
    float top = 1080 - margin - height;
    

    auto move = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new MoveTool(inputDevice, buffers.unlit, font, camera, world)));
    move->name = "move";
    move->button.bounds = FloatRect(margin, top, margin+width, top + height);
    move->button.text = "1. Move";
    move->button.toggled = false;
    ui->add(&move->button);
    move->inputId = INPUT_ACTION_TOOL_1;
    margin += move->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(move));


    auto terrain = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new TerrainTool(inputDevice, buffers.unlit, font, camera, world)));
    terrain->name = "terrain";
    terrain->button.bounds = FloatRect(margin, top, margin+width, top + height);
    terrain->button.text = "2. Terrain";
    terrain->button.toggled = false;
    ui->add(&terrain->button);
    terrain->inputId = INPUT_ACTION_TOOL_2;
    margin += terrain->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(terrain));

    auto ladder = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new LadderTool(inputDevice, buffers.unlit, font, camera, world)));
    ladder->name = "Ladder";
    ladder->button.bounds = FloatRect(margin, top, margin+width, top + height);
    ladder->button.text = "3. Ladder";
    ladder->button.toggled = false;
    ui->add(&ladder->button);
    ladder->inputId = INPUT_ACTION_TOOL_3;
    margin += ladder->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(ladder));

    auto image = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new ImageTool(inputDevice, buffers.unlit, font, camera, world)));
    image->name = "Image";
    image->button.bounds = FloatRect(margin, top, margin+width, top + height);
    image->button.text = "4. Image";
    image->button.toggled = false;
    ui->add(&image->button);
    image->inputId = INPUT_ACTION_TOOL_4;
    margin += image->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(image));

    auto platform = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new PlatformTool(inputDevice, buffers.unlit, font, camera, world)));
    platform->name = "platform";
    platform->button.bounds = FloatRect(margin, top, margin+width, top + height);
    platform->button.text = "5. Platform";
    platform->button.toggled = false;
    ui->add(&platform->button);
    platform->inputId = INPUT_ACTION_TOOL_5;
    margin += platform->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(platform));

    auto path = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new PathTool(inputDevice, buffers.unlit, font, camera, world)));
    path->name = "path";
    path->button.bounds = FloatRect(margin, top, margin+width, top + height);
    path->button.text = "6. Path";
    path->button.toggled = false;
    ui->add(&path->button);
    path->inputId = INPUT_ACTION_TOOL_6;
    margin += path->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(path));

    auto light = std::make_unique<ToolbarItem>(std::unique_ptr<ITool>(new LightTool(inputDevice, buffers, font, camera, world)));
    light->name = "light";
    light->button.bounds = FloatRect(margin, top, margin+width, top + height);
    light->button.text = "7. Light";
    light->button.toggled = false;
    ui->add(&light->button);
    light->inputId = 0;
    margin += light->button.bounds.width() + 4;
    toolbar.emplace_back(std::move(light));
}

void Editor::selectToolByInputId(MappedId inputId) {
    for(i32 i = 0; i < toolbar.size(); ++i) {
        auto& item = toolbar[i];
        if(item->inputId == inputId && selectedTool != i) {
            selectedTool = i;
            resetTools();
        }
        item->button.toggled = i == selectedTool;
    }
}

void Editor::update(float deltaTime) {
    bool ignoreClick = false;

    ui->processInput();

    for(i32 i = 0; i < toolbar.size(); ++i) {
        auto& item = toolbar[i];
        if(ui->wasClicked(&item->button)) {
            ignoreClick = true;
            if(i != selectedTool) {
                selectedTool = i;
                resetTools();
            }
        }
        item->button.toggled = i == selectedTool;
    }

    // dispatch input
    Input::Action action = {};
    auto &mouse = inputDevice.getMouseState();
    while(inputContext->pollAction(action)) {

        switch(action.id) {
            case INPUT_ACTION_LMB_DOWN: {
                if(!ignoreClick) {
                    onLeftDown(mouse.x, mouse.y);
                }
                break;
            }
            case INPUT_ACTION_LMB_UP: {
                if(!ignoreClick) {
                    onLeftUp(mouse.x, mouse.y);
                }
                break;
            }
            case INPUT_ACTION_TOOL_1:
            case INPUT_ACTION_TOOL_2:
            case INPUT_ACTION_TOOL_3:
            case INPUT_ACTION_TOOL_4:
            case INPUT_ACTION_TOOL_5:
            case INPUT_ACTION_TOOL_6:{
                selectToolByInputId(action.id);
                break;
            }
            default: {
                if(!toolbar[selectedTool]->tool->onKeyboard(action.id)) {
                    if(action.id == INPUT_ACTION_DEL) {
                        onDelete();
                    }

                    if(action.id == INPUT_ACTION_DUPLICATE && selected) {
                        duplicate(selected);
                    }
                    if(action.id == INPUT_ACTION_GRID) {
                        cycleGridSize();
                    }
                }
                break;
            }
        }
    }

    auto newSelected = toolbar[selectedTool]->tool->getNewSelected();
    if(newSelected) {
        setSelected(newSelected);
    }

    toolbar[selectedTool]->tool->onUpdate();

    float margin = 8.0f;
    FloatRect levelRect(-camera.scrollX, -camera.scrollY, -camera.scrollX + camera.levelWidth, -camera.scrollY + camera.levelHeight);
    buffers.unlit.pushRect(levelRect, RED);
    buffers.unlit.pushText(string_format("%.2f,%.2f", 0.0f, 0.0f), &font, levelRect.left, levelRect.top - margin, WHITE);
    std::string text = string_format("%.2f,%.2f", camera.levelWidth, 0.0f);
    buffers.unlit.pushText(text, &font, levelRect.right - font.measureTextWidth(text), levelRect.top - margin, WHITE);
    text = string_format("%.2f,%.2f", 0.0f, camera.levelHeight);
    buffers.unlit.pushText(text, &font, levelRect.left, levelRect.bottom + margin + font.getSize(), WHITE);

    text = string_format("%.2f,%.2f", camera.levelWidth, camera.levelHeight);
    buffers.unlit.pushText(text, &font, levelRect.right - font.measureTextWidth(text), levelRect.bottom + margin + font.getSize(), WHITE);

    text = string_format("GridSize: %d", getGridSizes()[currentGridSize]);
    buffers.unlit.pushText(text, &font, 1920.0f - font.measureTextWidth(text) - margin, 1080.0f - margin, WHITE);

    ui->render(deltaTime);
}


void Editor::cycleGridSize() {
    currentGridSize++;
    auto &gridSizes = getGridSizes();
    if(currentGridSize > gridSizes.size() - 1) {
        currentGridSize = 0;
    }
    setGridSize(currentGridSize);
}

void Editor::reset() {
    setSelected(nullptr);
    selectedTool = 0;
    resetTools();
}

void Editor::resetTools() {
   for(auto& item : toolbar) {
       item->tool->reset();
   }
}

void Editor::setSelected(Entity *ent) {
    selected = ent;
    for(auto& item : toolbar) {
        item->tool->setSelected(ent);
    }
}

void Editor::duplicate(Entity *ent) {
    auto newEntity = world->create();
    auto oldTerrain = ent->get<TerrainComponent>();
    auto oldTransform = ent->get<TransformComponent>();
    auto oldPlatform = ent->get<PlatformComponent>();
    auto oldPath = ent->get<PathComponent>();
    if(oldPlatform.isValid()) {
        newEntity->assign<TransformComponent>(oldTransform.get());
        newEntity->assign<TerrainComponent>();
        newEntity->assign<PlatformComponent>(oldPlatform.get());
        newEntity->assign<CollisionComponent>();
        rebuildMovingPlatform(newEntity);
    } else if(oldTerrain.isValid()) {
        newEntity->assign<TerrainComponent>(oldTerrain->polygon, oldTerrain->show);
        auto newTerrain = newEntity->get<TerrainComponent>();
        newTerrain->rebuildMesh();
    }
    auto oldLadder = ent->get<LadderComponent>();
    if(oldLadder.isValid()) {
        newEntity->assign<LadderComponent>(oldLadder->rect);
    }
    auto oldImage = ent->get<ImageComponent>();
    if(oldImage.isValid() && oldTransform.isValid()) {
        newEntity->assign<TransformComponent>(oldTransform.get());
        newEntity->assign<ImageComponent>(oldImage.get());
    }
    if(oldPath.isValid()) {
        newEntity->assign<PathComponent>(oldPath.get());
    }
    setSelected(newEntity);
}

void Editor::onDelete() {
    if(selected) {
        world->destroy(selected, true);
        setSelected(nullptr);
    }
}

void Editor::onLeftDown(float x, float y) {
    Vector2 pos(x + camera.scrollX, y + camera.scrollY);
    SDL_Log("onLeftDown %.2f,%.2f", pos.x, pos.y);

    if(!toolbar[selectedTool]->tool->onLeftDown(pos)) {
        bool clickedNothing = true;
        for (Entity *ent : world->all(false)) {
            auto terrain = ent->get<TerrainComponent>();
            auto ladder = ent->get<LadderComponent>();
            auto image = ent->get<ImageComponent>();
            auto transform = ent->get<TransformComponent>();
            auto light = ent->get<PointLightComponent>();
            if(image.isValid() && transform.isValid()) {
                FloatRect imageRect;
                imageRect.left = transform->pos.x;
                imageRect.top = transform->pos.y;
                imageRect.right = transform->pos.x + (transform->scale * image->width);
                imageRect.bottom = transform->pos.y + (transform->scale * image->height);
                if(imageRect.containsPoint(pos.x, pos.y)) {
                    clickedNothing = false;
                    if(ent != selected) {
                        setSelected(ent);
                        break;
                    }
                }
            }
            if(terrain.isValid()) {
                if(pointInPolygon(pos, terrain->polygon)) {
                    clickedNothing = false;
                    if(ent != selected) {
                        setSelected(ent);
                        break;
                    }
                }
            }
            if(ladder.isValid()) {
                if(ladder->rect.containsPoint(pos.x, pos.y)) {
                    clickedNothing = false;
                    if(ent != selected) {
                        setSelected(ent);
                        break;
                    }
                }
            }
            if(light.isValid() && transform.isValid()) {
                if(pointInCircle(pos, transform->pos, light->outerRadius)) {
                    clickedNothing = false;
                    if(ent != selected) {
                        setSelected(ent);
                        break;
                    }
                }
            }
        }
        if(clickedNothing) {
            SDL_Log("clicked nothing");
            setSelected(nullptr);
        }
    }
}

void Editor::onLeftUp(float x, float y) {
    Vector2 pos(x + camera.scrollX, y + camera.scrollY);
    SDL_Log("onLeftUp %.2f,%.2f", pos.x, pos.y);

    toolbar[selectedTool]->tool->onLeftUp(pos);
}

