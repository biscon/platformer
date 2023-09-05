cmake_minimum_required(VERSION 3.8)
project(platformer)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -Wall -fsanitize=undefined -fsanitize=address -fsanitize=leak -fsanitize=integer-divide-by-zero -fsanitize=null -fsanitize=signed-integer-overflow -fsanitize=bounds -fstack-protector")
#set(CMAKE_C_FLAGS_DEBUG "-g -Wall -fsanitize=leak -fsanitize=integer-divide-by-zero -fsanitize=null -fsanitize=signed-integer-overflow -fsanitize=bounds -fstack-protector")
#set(CMAKE_C_FLAGS_DEBUG "-g -Wall -fsanitize=leak")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "-O3 -Wall -funroll-loops -fomit-frame-pointer -ftree-loop-distribution -ftree-vectorize")
# uncomment this to compile as gui app (doesnt open the console)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

if(APPLE)
set(CMAKE_EXE_LINKER_FLAGS "-L/opt/homebrew/Cellar/sdl2/2.26.2/lib -lSDL2")
#    set(CMAKE_CXX_FLAGS "-framework OpenGL")
# -framework OpenGL -framework CoreVideo
#    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# includes cmake/FindSDL2.cmake
#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(UNIX AND NOT APPLE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--copy-dt-needed-entries")
endif()
#if(WIN32)
#endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../")
find_package(ZLIB REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Freetype REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/include ${ZLIB_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/imgui)
#message(0, ${SDL2_MIXER_LIBRARIES})

set(CGLM_SOURCE_FILES
        src/util/cglm/affine.c
        src/util/cglm/box.c
        src/util/cglm/cam.c
        src/util/cglm/ease.c
        src/util/cglm/euler.c
        src/util/cglm/frustum.c
        src/util/cglm/io.c
        src/util/cglm/mat3.c
        src/util/cglm/mat4.c
        src/util/cglm/plane.c
        src/util/cglm/project.c
        src/util/cglm/quat.c
        src/util/cglm/sphere.c
        src/util/cglm/vec3.c
        src/util/cglm/vec4.c)

set(IM_GUI_SOURCE_FILES
        src/imgui/imgui_impl_opengl3.cpp
        src/imgui/imgui_impl_sdl.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_stdlib.cpp)

set(UTIL_SOURCE_FILES
        src/util/string_util.cpp
        src/util/string_util.h
        src/include/stb_rect_pack.h
        src/include/earcut.h
        src/include/json.hpp
        src/include/RenderBuffers.h
        src/include/Vector2.h
        src/util/lodepng.h
        src/util/lodepng.c
        src/util/math_util.c
        src/util/math_util.h
        src/util/ImGuiFileDialog.h
        src/util/ImGuiFileDialog.cpp
        src/util/ComponentVector.h)

set(ENGINE_SOURCE_FILES
        src/include/defs.h
        src/main.cpp
        src/glad/glad.c
        src/renderer/WindEffectParameters.h
        src/renderer/RenderCmdBuffer.cpp
        src/renderer/RenderCmdBuffer.h
        src/renderer/Commands.cpp
        src/renderer/Commands.h
        src/renderer/IRenderDevice.h
        src/renderer/OGLRenderDevice.cpp
        src/renderer/OGLRenderDevice.h
        src/renderer/PixelBuffer.cpp
        src/renderer/PixelBuffer.h
        src/renderer/Types.h
        src/renderer/TextureAtlas.cpp
        src/renderer/TextureAtlas.h
        src/renderer/Font.cpp
        src/renderer/Font.h
        src/input/mapped_input.h
        src/input/Input.cpp
        src/input/Input.h
        src/input/SDLInputDevice.cpp
        src/input/SDLInputDevice.h
        src/renderer/Animation.cpp
        src/renderer/Animation.h
        src/renderer/Gui.cpp
        src/renderer/Gui.h
        src/renderer2/Renderer.cpp
        src/renderer2/Renderer.h
        src/renderer2/ShaderProgram.cpp
        src/renderer2/ShaderProgram.h
        src/renderer2/VertexBuffer.cpp
        src/renderer2/VertexBuffer.h
        src/renderer2/FrameBuffer.cpp
        src/renderer2/FrameBuffer.h
        src/renderer2/Texture.cpp
        src/renderer2/Texture.h
)

set(GAME_SOURCE_FILES
        src/include/ECS.h
        src/game/Game.cpp
        src/game/Game.h
        src/game/Level.cpp
        src/game/Level.h
        src/game/Utils.cpp
        src/game/Utils.h
        src/game/AABB.cpp
        src/game/AABB.h
        src/game/PolyUtil.cpp
        src/game/PolyUtil.h
        src/game/Camera.cpp
        src/game/Camera.h
        src/game/ParallaxScroller.cpp
        src/game/ParallaxScroller.h
        src/game/JsonUtil.cpp
        src/game/JsonUtil.h
        src/include/LevelConfig.h
        src/game/TransitionEffect.cpp
        src/game/TransitionEffect.h
        src/game/AnimationManager.cpp
        src/game/AnimationManager.h

        src/game/systems/InputSystem.cpp
        src/game/systems/InputSystem.h
        src/game/systems/RenderLevelSystem.cpp
        src/game/systems/RenderLevelSystem.h
        src/game/systems/UpdateAnimationSystem.cpp
        src/game/systems/UpdateAnimationSystem.h
        src/game/systems/RenderDebugSystem.cpp
        src/game/systems/RenderDebugSystem.h
        src/game/systems/ActorSystem.cpp
        src/game/systems/ActorSystem.h
        src/game/systems/CollisionSystem.cpp
        src/game/systems/CollisionSystem.h
        src/game/systems/RaycastSystem.cpp
        src/game/systems/RaycastSystem.h
        src/game/systems/PlatformSystem.cpp
        src/game/systems/PlatformSystem.h
        src/game/systems/VerletSystem.cpp
        src/game/systems/VerletSystem.h
        src/game/systems/UpdateEffectsSystem.cpp
        src/game/systems/UpdateEffectsSystem.h
        src/game/systems/DoorSystem.cpp
        src/game/systems/DoorSystem.h

        src/game/components/SpriteComponent.cpp
        src/game/components/SpriteComponent.h
        src/game/components/ActorComponent.cpp
        src/game/components/ActorComponent.h
        src/game/components/TerrainComponent.cpp
        src/game/components/TerrainComponent.h
        src/game/components/CollisionComponent.cpp
        src/game/components/CollisionComponent.h
        src/game/components/ImageComponent.cpp
        src/game/components/ImageComponent.h
        src/game/components/PointLightComponent.cpp
        src/game/components/PointLightComponent.h
        src/game/components/PlatformComponent.cpp
        src/game/components/PlatformComponent.h
        src/game/components/TransformComponent.h
        src/game/components/PhysicComponent.h
        src/game/components/InputComponent.h
        src/game/components/CollisionInfoComponent.h
        src/game/components/LadderComponent.h
        src/game/components/PathComponent.h
        src/game/components/VerletMeshComponent.cpp
        src/game/components/VerletMeshComponent.h
        src/game/components/FlickerEffectComponent.h
        src/game/components/GlowEffectComponent.h
        src/game/components/WindEffectComponent.h
        src/game/components/DoorComponent.h

        src/game/editor/Editor.cpp
        src/game/editor/Editor.h
        src/game/editor/ITool.h
        src/game/editor/MoveTool.cpp
        src/game/editor/MoveTool.h
        src/game/editor/TerrainTool.cpp
        src/game/editor/TerrainTool.h
        src/game/editor/ToolUtil.cpp
        src/game/editor/ToolUtil.h
        src/game/editor/RectangleTool.cpp
        src/game/editor/RectangleTool.h
        src/game/editor/PlatformTool.cpp
        src/game/editor/PlatformTool.h
        src/game/editor/PathTool.cpp
        src/game/editor/PathTool.h
        src/game/editor/IComponentPropertyEditor.h
        src/game/editor/TransformPropertyEditor.cpp
        src/game/editor/TransformPropertyEditor.h
        src/game/editor/TerrainPropertyEditor.cpp
        src/game/editor/TerrainPropertyEditor.h
        src/game/editor/ImagePropertyEditor.cpp
        src/game/editor/ImagePropertyEditor.h
        src/game/editor/shared/RectanglePropertyEditor.cpp
        src/game/editor/shared/RectanglePropertyEditor.h
        src/game/editor/LightPropertyEditor.cpp
        src/game/editor/LightPropertyEditor.h
        src/game/editor/LadderPropertyEditor.cpp
        src/game/editor/LadderPropertyEditor.h
        src/game/editor/SpritePropertyEditor.cpp
        src/game/editor/SpritePropertyEditor.h
        src/game/editor/WindEffectPropertyEditor.cpp
        src/game/editor/WindEffectPropertyEditor.h
        src/game/editor/DoorPropertyEditor.cpp
        src/game/editor/DoorPropertyEditor.h
)

add_executable(game ${GAME_SOURCE_FILES} ${ENGINE_SOURCE_FILES} ${UTIL_SOURCE_FILES} ${IM_GUI_SOURCE_FILES} ${CGLM_SOURCE_FILES})

target_link_libraries(game m ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES} ${FREETYPE_LIBRARIES} ${ZLIB_LIBRARIES})