
#include <stdio.h>
#include <SDL.h>
#include <glad.h>
#include <SDL_video.h>
#include <assert.h>
#include <SDL_mixer.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <memory>
#include <RenderBuffers.h>

extern "C" {
    #include "defs.h"
    #include "util/math_util.h"
}

#include "input/SDLInputDevice.h"
#include "renderer/IRenderDevice.h"
#include "renderer/OGLRenderDevice.h"
#include "renderer/Font.h"
#include "game/Game.h"


global_variable u32 ScreenWidth = 1920;
global_variable u32 ScreenHeight = 1080;
global_variable SDL_Window* SDLWindow = nullptr;
global_variable SDL_GLContext Context = nullptr;
global_variable bool ShouldQuit = false;
global_variable u64 InitTimeStamp = 0;
global_variable u64 PerformanceFrequency = 0;

double GetTime() {
    return ((double) (SDL_GetPerformanceCounter() - InitTimeStamp)) / (double) PerformanceFrequency;
}

// Internal functions ------------------------------------------------------------------------------

INTERNAL void HandleKeyboardEvent(const SDL_Event *event, Input::SDLInputDevice &inputDevice)
{
    switch( event->type ) {
        case SDL_KEYDOWN: {
            if(event->key.repeat == 0) {
                inputDevice.onPushKeyDown(event->key.keysym.sym);
            }
            break;
        }
        case SDL_KEYUP: {
            inputDevice.onPushKeyUp(event->key.keysym.sym);
            break;
        }
        default:break;
    }
}


INTERNAL void HandleMouseEvent(const SDL_Event *event, Input::SDLInputDevice &inputDevice, Renderer::OGLRenderDevice& renderDevice)
{
    switch( event->type ) {
        case SDL_MOUSEMOTION: {
            SDL_Rect *vp = renderDevice.getCurrentViewPort();
            SDL_Point p = { event->motion.x, event->motion.y };
            if(SDL_PointInRect(&p, vp)) {
                // to viewport coords
                i32 vpx = p.x - vp->x;
                i32 vpy = p.y - vp->y;
                // to screen res
                float x = remapFloat(0, (float) vp->w, 0, (float) ScreenWidth, (float) vpx);
                float y = remapFloat(0, (float) vp->h, 0, (float) ScreenHeight, (float) vpy);
                inputDevice.onMouseMotion(x, y);
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if(event->button.button == SDL_BUTTON_LEFT) {
                inputDevice.onMouseLeftButton(true);
            }
            if(event->button.button == SDL_BUTTON_RIGHT) {
                inputDevice.onMouseRightButton(true);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            if(event->button.button == SDL_BUTTON_LEFT) {
                inputDevice.onMouseLeftButton(false);
            }
            if(event->button.button == SDL_BUTTON_RIGHT) {
                inputDevice.onMouseRightButton(false);
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            break;
        }
        default:break;
    }
}

INTERNAL void HandleWindowEvent(const SDL_Event *event, Renderer::OGLRenderDevice& renderDevice)
{
    switch( event->window.event ) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            // call listeners
            renderDevice.viewportSizeChanged(event->window.data1, event->window.data2);
            break;
        }
        case SDL_WINDOWEVENT_ENTER: {

            break;
        }
        case SDL_WINDOWEVENT_LEAVE: {

            break;
        }
        case SDL_WINDOWEVENT_FOCUS_GAINED: {

            break;
        }
        case SDL_WINDOWEVENT_FOCUS_LOST: {

            break;
        }
        default:break;
    }
}

INTERNAL void UpdateInput(Input::SDLInputDevice &inputDevice, Renderer::OGLRenderDevice& renderDevice) {
    //Handle events on queue
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //SDL_Log("wantKey: %d, wantMouse: %d, wantTextInput: %d", io.WantCaptureKeyboard, io.WantCaptureMouse, io.WantTextInput);
        ImGui_ImplSDL2_ProcessEvent(&e);


        //SDL_Log("processed: %d", processed);

        //User requests quit
        if( e.type == SDL_QUIT )
        {
            ShouldQuit = true;
            return;
        }
        if(!io.WantCaptureKeyboard) {
            HandleKeyboardEvent(&e, inputDevice);
        }
        if(!io.WantCaptureMouse) {
            if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL ||
                e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
                HandleMouseEvent(&e, inputDevice, renderDevice);
            }
        }
        if(e.type == SDL_WINDOWEVENT)
        {
            //SDL_Log("SDLWindow event");
            HandleWindowEvent(&e, renderDevice);
        }
    }
}

INTERNAL bool InitVideo()
{
    // request a GL Context 3.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // stencil size 8, so far we don't use it
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // request 4x MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDLWindow = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    if(SDLWindow == nullptr)
    {
        SDL_Log("Could not create SDLWindow\n");
        return false;
    }
    SDL_SetWindowFullscreen(SDLWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

    Context = SDL_GL_CreateContext(SDLWindow);
    if(Context == nullptr)
    {
        SDL_Log("Could not create Context\n");
        return false;
    }

    // request vsync
    SDL_GL_SetSwapInterval(1);

    // Load OpenGL functions glad SDL
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_Log("Vendor:          %s\n", glGetString(GL_VENDOR));
    SDL_Log("Renderer:        %s\n", glGetString(GL_RENDERER));
    SDL_Log("Version OpenGL:  %s\n", glGetString(GL_VERSION));
    SDL_Log("Version GLSL:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f, 1.0f);
    glEnable(GL_BLEND);

    glViewport(0, 0, ScreenWidth, ScreenHeight);
    return true;
}

INTERNAL void ShutdownVideo()
{
    if(Context != nullptr)
        SDL_GL_DeleteContext(Context);
    if(SDLWindow != nullptr)
        SDL_DestroyWindow(SDLWindow);
}

INTERNAL bool InitAudio() {
    // Set up the audio stream
    int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        return false;
    }

    result = Mix_AllocateChannels(4);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

INTERNAL void ShutdownAudio() {

}


struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };

void imgui_easy_theming(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops)
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 1.00f );
    style.Colors[ImGuiCol_TextDisabled] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.58f );
    style.Colors[ImGuiCol_WindowBg] = ImVec4( color_for_body.x, color_for_body.y, color_for_body.z, 0.95f );
    style.Colors[ImGuiCol_ChildBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.58f );
    style.Colors[ImGuiCol_Border] = ImVec4( color_for_body.x, color_for_body.y, color_for_body.z, 0.00f );
    style.Colors[ImGuiCol_BorderShadow] = ImVec4( color_for_body.x, color_for_body.y, color_for_body.z, 0.00f );
    style.Colors[ImGuiCol_FrameBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_TitleBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.75f );
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.47f );
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.21f );
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_CheckMark] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.80f );
    style.Colors[ImGuiCol_SliderGrab] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.50f );
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_Button] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.50f );
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.86f );
    style.Colors[ImGuiCol_ButtonActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_Header] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.76f );
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.86f );
    style.Colors[ImGuiCol_HeaderActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    /*
    style.Colors[ImGuiCol_Column] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.32f );
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
    style.Colors[ImGuiCol_ColumnActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
     */
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.15f );
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_PlotLines] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.63f );
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.63f );
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.43f );
    style.Colors[ImGuiCol_PopupBg] = ImVec4( color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f );
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.73f );
}

void setupImGuiStyle2()
{
    static ImVec3 color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
    static ImVec3 color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
    static ImVec3 color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
    static ImVec3 color_for_body = ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
    static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
}

/**
 * Entry point
 * @return
 */

int main()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return -1;
    }

    if(!InitVideo())
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return -1;
    }


    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGui_ImplSDL2_InitForOpenGL(SDLWindow, Context);
    const char* glsl_version = "#version 330 core";
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup style
    ImGui::StyleColorsDark();
    setupImGuiStyle2();
    //ImGui::StyleColorsClassic();
    io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Semibold.ttf", 20.0f);

    // get info about the current display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
        ShutdownVideo();
        SDL_Quit();
        return -1;
    }
    SDL_Log("Monitor refresh rate is %d hz", displayMode.refresh_rate);

    //i32 monitorRefreshHz = displayMode.refresh_rate;
    //i32 gameUpdateHz = monitorRefreshHz;
    i32 gameUpdateHz = 60;
    double targetSecondsPerFrame = 1.0 / (double) gameUpdateHz;
    PerformanceFrequency = SDL_GetPerformanceFrequency();
    InitTimeStamp = SDL_GetPerformanceCounter();
    double oldTime = 0;


    auto inputDevice = std::make_unique<Input::SDLInputDevice>();
    auto fontLibrary = std::make_unique<Renderer::FontLibrary>();

    double secondsElapsedForFrame = 0;

    auto renderDevice = std::make_unique<Renderer::OGLRenderDevice>(ScreenWidth, ScreenHeight);
    auto backgroundBuffer = std::make_unique<Renderer::RenderCmdBuffer>();
    auto litBuffer = std::make_unique<Renderer::RenderCmdBuffer>();
    auto unlitBuffer = std::make_unique<Renderer::RenderCmdBuffer>();
    auto lightBuffer = std::make_unique<Renderer::RenderCmdBuffer>();

    RenderBuffers renderBuffers = {
            .background = *backgroundBuffer,
            .lit = *litBuffer,
            .unlit = *unlitBuffer,
            .light = *lightBuffer,
    };

    auto game = std::make_unique<Game>(*inputDevice, *renderDevice, renderBuffers);

    while(!ShouldQuit)
    {
        oldTime = GetTime();
        UpdateInput(*inputDevice, *renderDevice);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(SDLWindow);
        ImGui::NewFrame();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            SDL_Log("Loop OpenGL error: %d", err);
        }

        //secondsElapsedForFrame = 1.0 / (double) 350;
        if(secondsElapsedForFrame > 0.5) {
            secondsElapsedForFrame = targetSecondsPerFrame;
            SDL_Log("Correcting crazy framerate");
        }

        renderBuffers.background.clear();
        renderBuffers.lit.clear();
        renderBuffers.unlit.clear();
        renderBuffers.light.clear();

        ShouldQuit = game->update(secondsElapsedForFrame);
        renderDevice->renderBackgroundBuffer(*backgroundBuffer);
        renderDevice->renderLightBuffer(*lightBuffer);
        renderDevice->renderLitBuffer(*litBuffer);
        renderDevice->renderUnlitBuffer(*unlitBuffer);
        renderDevice->renderScreen();


        // ImGui Rendering
        ImGui::Render();
        SDL_GL_MakeCurrent(SDLWindow, Context);
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        SDL_GL_SwapWindow(SDLWindow);
        //SDL_Delay(35);
        secondsElapsedForFrame = GetTime() - oldTime;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    ShutdownVideo();
    SDL_Quit();
    return 0;
}