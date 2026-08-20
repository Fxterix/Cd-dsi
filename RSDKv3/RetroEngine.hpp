#include "RetroEngine.hpp"
#if RETRO_PLATFORM == RETRO_UWP
#include <winrt/base.h>
#include <winrt/Windows.Storage.h>
#endif

#if RETRO_PLATFORM == RETRO_ANDROID
#include <unistd.h>
#endif

#if RETRO_PLATFORM == RETRO_DS
#include <nds.h>
#endif

bool usingCWD        = false;
bool engineDebugMode = false;
byte renderType      = RENDER_SW;

RetroEngine Engine = RetroEngine();

inline int GetLowerRate(int intendRate, int targetRate)
{
    int result   = 0;
    int valStore = 0;

    result = targetRate;
    if (intendRate) {
        do {
            valStore   = result % intendRate;
            result     = intendRate;
            intendRate = valStore;
        } while (valStore);
    }
    return result;
}

bool ProcessEvents()
{
#if RETRO_PLATFORM == RETRO_DS
    scanKeys();
    int keys = keysHeld();

    // Mapeo de botones físicos de la DSi a los estados del motor
    keyState[SDLK_UP]    = (keys & KEY_UP) ? 1 : 0;
    keyState[SDLK_DOWN]  = (keys & KEY_DOWN) ? 1 : 0;
    keyState[SDLK_LEFT]  = (keys & KEY_LEFT) ? 1 : 0;
    keyState[SDLK_RIGHT] = (keys & KEY_RIGHT) ? 1 : 0;
    keyState[SDLK_RETURN]= (keys & KEY_START) ? 1 : 0;
    keyState[SDLK_SPACE] = (keys & KEY_A) ? 1 : 0;
    keyState[SDLK_LSHIFT]= (keys & KEY_B) ? 1 : 0;

    if (keys & KEY_SELECT) {
        Engine.gameMode = ENGINE_EXITGAME;
        return false;
    }
##elif RETRO_USING_SDL1 || RETRO_USING_SDL2
    while (SDL_PollEvent(&Engine.sdlEvents)) {
        // Main Events
        switch (Engine.sdlEvents.type) {
#if RETRO_USING_SDL2
            case SDL_WINDOWEVENT:
                switch (Engine.sdlEvents.window.event) {
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        SDL_RestoreWindow(Engine.window);
                        SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        Engine.isFullScreen = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: Engine.gameMode = ENGINE_EXITGAME; return false;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        if (!((disableFocusPause + 1) & 1))
                            Engine.message = MESSAGE_LOSTFOCUS;
                        Engine.hasFocus = false;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED: Engine.hasFocus = true; break;
                }
                break;
            case SDL_CONTROLLERDEVICEADDED: ControllerInit(Engine.sdlEvents.cdevice.which); break;
            case SDL_CONTROLLERDEVICEREMOVED: ControllerClose(Engine.sdlEvents.cdevice.which); break;
            case SDL_APP_WILLENTERBACKGROUND:
                if (!((disableFocusPause + 1) & 1))
                    Engine.message = MESSAGE_LOSTFOCUS;
                Engine.hasFocus = false;
                break;
            case SDL_APP_WILLENTERFOREGROUND: Engine.hasFocus = true; break;
            case SDL_APP_TERMINATING: Engine.gameMode = ENGINE_EXITGAME; return false;

#endif

#if defined(RETRO_USING_MOUSE) && RETRO_USING_SDL2
            case SDL_MOUSEMOTION:
                if (touches <= 1) { // Touch always takes priority over mouse
                    uint state = SDL_GetMouseState(&touchX[0], &touchY[0]);

                    int width = 0, height = 0, pixW = 0, pixH = 0;
                    SDL_GetWindowSize(Engine.window, &width, &height);
#if RETRO_USING_OPENGL
                    SDL_GL_GetDrawableSize(Engine.window, &pixW, &pixH);
#else
                    SDL_GetRendererOutputSize(Engine.renderer, &pixW, &pixH);
#endif

                    float scaleX = (float)pixW / (float)width;
                    float scaleY = (float)pixH / (float)height;

                    touchX[0] = ((touchX[0] - viewOffsetX / scaleX) / ((float)width - viewOffsetX)) * SCREEN_XSIZE;
                    touchY[0] = ((touchY[0] - viewOffsetY / scaleY) / ((float)height - viewOffsetY)) * SCREEN_YSIZE;

                    touchDown[0] = state & SDL_BUTTON_LMASK;
                    if (touchDown[0])
                        touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (touches <= 1) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = true; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (touches <= 1) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = false; break;
                    }
                    touches = 0;
                }
                break;
#endif

#if RETRO_USING_SDL2 && defined(RETRO_USING_TOUCH)
            case SDL_FINGERMOTION:
            case SDL_FINGERDOWN:
            case SDL_FINGERUP: {
                int count = SDL_GetNumTouchFingers(Engine.sdlEvents.tfinger.touchId);
                touches   = 0;
                for (int i = 0; i < count; i++) {
                    SDL_Finger *finger = SDL_GetTouchFinger(Engine.sdlEvents.tfinger.touchId, i);
                    if (finger) {
                        touchDown[touches] = true;
                        touchX[touches]    = finger->x * SCREEN_XSIZE;
                        touchY[touches]    = finger->y * SCREEN_YSIZE;
                        touches++;
                    }
                }
                break;
            }
#endif
            case SDL_KEYDOWN:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;

                    case SDLK_ESCAPE:
                        if (Engine.devMenu) {
#if RETRO_USE_MOD_LOADER
                            // hacky patch because people can escape
                            if (Engine.gameMode == ENGINE_DEVMENU && stageMode == DEVMENU_MODMENU) {
                                RefreshEngine();
                            }
#endif

                            Engine.gameMode = ENGINE_INITDEVMENU;
                        }
                        break;

                    case SDLK_F1:
                        if (Engine.devMenu) {
                            activeStageList   = 0;
                            stageListPosition = 0;
                            stageMode         = STAGEMODE_LOAD;
                            Engine.gameMode   = ENGINE_MAINGAME;
                            if (Engine.highResMode)
                                Engine.highResMode = false;
                        }
                        break;

                    case SDLK_F2:
                        if (Engine.devMenu) {
                            stageListPosition--;
                            if (stageListPosition < 0) {
                                activeStageList--;

                                if (activeStageList < 0) {
                                    activeStageList = 3;
                                }
                                stageListPosition = stageListCount[activeStageList] - 1;
                            }
                            stageMode       = STAGEMODE_LOAD;
                            Engine.gameMode = ENGINE_MAINGAME;
                            SetGlobalVariableByName("LampPost.Check", 0);
                            SetGlobalVariableByName("Warp.XPos", 0);
                            if (Engine.highResMode)
                                Engine.highResMode = false;
                        }
                        break;

                    case SDLK_F3:
                        if (Engine.devMenu) {
                            stageListPosition++;
                            if (stageListPosition >= stageListCount[activeStageList]) {
                                activeStageList++;

                                stageListPosition = 0;

                                if (activeStageList >= 4) {
                                    activeStageList = 0;
                                }
                            }
                            stageMode       = STAGEMODE_LOAD;
                            Engine.gameMode = ENGINE_MAINGAME;
                            SetGlobalVariableByName("LampPost.Check", 0);
                            SetGlobalVariableByName("Warp.XPos", 0);
                            if (Engine.highResMode)
                                Engine.highResMode = false;
                        }
                        break;

                    case SDLK_F4:
                        Engine.isFullScreen ^= 1;
                        SetFullScreen(Engine.isFullScreen);
                        break;

                    case SDLK_F5:
                        if (Engine.devMenu) {
                            currentStageFolder[0] = 0; // reload all assets & scripts
                            stageMode             = STAGEMODE_LOAD;
                        }
                        break;

                    case SDLK_F8:
                        if (Engine.devMenu)
                            showHitboxes ^= 2;
                        break;

                    case SDLK_F9:
                        if (Engine.devMenu)
                            showHitboxes ^= 1;
                        break;

                    case SDLK_F10:
                        if (Engine.devMenu)
                            Engine.showPaletteOverlay ^= 1;
                        break;

                    case SDLK_BACKSPACE:
                        if (Engine.devMenu)
                            Engine.gameSpeed = Engine.fastForwardSpeed;
                        break;

#if RETRO_PLATFORM == RETRO_OSX
                    case SDLK_F6:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;

                    case SDLK_F7:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#else
                    case SDLK_F11:
                    case SDLK_INSERT:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;

                    case SDLK_F12:
                    case SDLK_PAUSE:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#endif
                }

#if RETRO_USING_SDL1
                keyState[Engine.sdlEvents.key.keysym.sym] = 1;
#endif
                break;
            case SDL_KEYUP:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_BACKSPACE: Engine.gameSpeed = 1; break;
                }
#if RETRO_USING_SDL1
                keyState[Engine.sdlEvents.key.keysym.sym] = 0;
#endif
                break;
            case SDL_QUIT: Engine.gameMode = ENGINE_EXITGAME; return false;
        }
    }
#endif
    return true;
}

void RetroEngine::Init()
{
    CalculateTrigAngles();
    GenerateBlendLookupTable();
    InitUserdata();
#if RETRO_USE_MOD_LOADER
    InitMods();
#endif
    char dest[0x200];
#if RETRO_PLATFORM == RETRO_UWP
    static char resourcePath[256] = { 0 };

    if (strlen(resourcePath) == 0) {
        auto folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        auto path   = to_string(folder.Path());

        std::copy(path.begin(), path.end(), resourcePath);
    }

    strcat(dest, resourcePath);
    strcat(dest, "\\");
    strcat(dest, Engine.dataFile);
#elif RETRO_PLATFORM == RETRO_ANDROID
    StrCopy(dest, gamePath);
    StrAdd(dest, Engine.dataFile);
    disableFocusPause = 0; // focus pause is ALWAYS enabled.
#else
    StrCopy(dest, BASE_PATH);
    StrAdd(dest, Engine.dataFile);
#endif
    CheckRSDKFile(dest);

    Engine.useFBTexture = Engine.scalingMode;

    gameMode = ENGINE_EXITGAME;
    running  = false;
    if (LoadGameConfig("Data/Game/GameConfig.bin")) {
        if (InitRenderDevice()) {
            if (InitAudioPlayback()) {
                InitFirstStage();
                ClearScriptData();
                initialised = true;
                running     = true;
                gameMode    = ENGINE_MAINGAME;
            }
        }
    }

    // Calculate Skip frame
    int lower        = GetLowerRate(targetRefreshRate, refreshRate);
    renderFrameIndex = targetRefreshRate / lower;
    skipFrameIndex   = refreshRate / lower;

#if !RETRO_USE_ORIGINAL_CODE
    // "error message"
    if (!running) {
        char rootDir[0x80];
        char pathBuffer[0x80];

#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(rootDir, "%s/", getResourcesPath());
        else
            sprintf(rootDir, "%s", "");
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(rootDir, "%s/", gamePath);
#else
        sprintf(rootDir, "%s", "");
#endif
        sprintf(pathBuffer, "%s%s", rootDir, "usage.txt");

        FileIO *f;
        if ((f = fOpen(pathBuffer, "w")) == NULL) {
            PrintLog("ERROR: Couldn't open file '%s' for writing!", "usage.txt");
            return;
        }

        char textBuf[0x100];
        sprintf(textBuf, "RETRO ENGINE v3 USAGE:\n");
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- Open the asset directory '%s' in a file browser\n", !rootDir[0] ? "./" : rootDir);
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- Place a data pack named '%s' in the asset directory\n", Engine.dataFile);
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- OR extract a data pack and place the \"Data\" folder in the asset directory\n");
        fWrite(textBuf, 1, strlen(textBuf), f);

        fClose(f);
    }
#endif
}

void RetroEngine::Run()
{
#if RETRO_PLATFORM == RETRO_DS
    while (running) {
        running = ProcessEvents();

        for (int s = 0; s < gameSpeed; ++s) {
            ProcessInput();

            if (!masterPaused || frameStep) {
                switch (gameMode) {
                    case ENGINE_DEVMENU:
                        ProcessStageSelect();
                        break;
                    case ENGINE_MAINGAME:
                        ProcessStage();
                        break;
                    case ENGINE_EXITGAME:
                        running = false;
                        break;
                    default:
                        break;
                }
            }
        }

        FlipScreen();
        swiWaitForVBlank();

        frameStep      = false;
        Engine.message = MESSAGE_NONE;
    }
#else
    unsigned long long targetFreq = SDL_GetPerformanceFrequency() / Engine.refreshRate;
    unsigned long long curTicks   = 0;
    unsigned long long prevTicks  = 0;

    while (running) {
#if !RETRO_USE_ORIGINAL_CODE
        if (!vsync) {
            curTicks = SDL_GetPerformanceCounter();
            if (curTicks < prevTicks + targetFreq)
                continue;
            prevTicks = curTicks;
        }
#endif
        running = ProcessEvents();

        // Focus Checks
        if (!((disableFocusPause + 1) & 2)) {
            if (!Engine.hasFocus) {
                if (!(Engine.focusState & 1))
                    Engine.focusState = PauseSound() ? 3 : 1;
            }
            else if (Engine.focusState) {
                if ((Engine.focusState & 2))
                    ResumeSound();
                Engine.focusState = 0;
            }
        }

        if (!(Engine.focusState & 1)) {
            for (int s = 0; s < gameSpeed; ++s) {
                ProcessInput();

                if (!masterPaused || frameStep) {
                    switch (gameMode) {
                        case ENGINE_DEVMENU:
                            if (renderType == RENDER_HW) {
                                gfxIndexSize        = 0;
                                gfxVertexSize       = 0;
                                gfxIndexSizeOpaque  = 0;
                                gfxVertexSizeOpaque = 0;
                            }
                            ProcessStageSelect();
                            break;

                        case ENGINE_MAINGAME:
                            if (renderType == RENDER_HW) {
                                gfxIndexSize        = 0;
                                gfxVertexSize       = 0;
                                gfxIndexSizeOpaque  = 0;
                                gfxVertexSizeOpaque = 0;
                                vertexSize3D        = 0;
                                indexSize3D         = 0;
                                render3DEnabled     = false;
                            }
                            ProcessStage();
                            break;

                        case ENGINE_INITDEVMENU:
                            LoadGameConfig("Data/Game/GameConfig.bin");
                            InitDevMenu();
                            ResetCurrentStageFolder();
                            break;

                        case ENGINE_EXITGAME: running = false; break;

                        case ENGINE_SCRIPTERROR:
                            LoadGameConfig("Data/Game/GameConfig.bin");
                            InitErrorMessage();
                            ResetCurrentStageFolder();
                            break;

                        case ENGINE_ENTER_HIRESMODE:
                            gameMode    = ENGINE_MAINGAME;
                            highResMode = true;
                            PrintLog("Callback: HiRes Mode Enabled");
                            break;

                        case ENGINE_EXIT_HIRESMODE:
                            gameMode    = ENGINE_MAINGAME;
                            highResMode = false;
                            PrintLog("Callback: HiRes Mode Disabled");
                            break;

                        case ENGINE_PAUSE: break;
                        case ENGINE_WAIT: break;

                        case ENGINE_VIDEOWAIT:
                            if (ProcessVideo() == 1)
                                gameMode = ENGINE_MAINGAME;
                            break;

                        default: break;
                    }
                }
            }
        }

        FlipScreen();

#if RETRO_USING_OPENGL && RETRO_USING_SDL2
        SDL_GL_SwapWindow(Engine.window);
#endif
        frameStep      = false;
        Engine.message = MESSAGE_NONE;
    }
#endif

    ReleaseAudioDevice();
    StopVideoPlayback();
    ReleaseRenderDevice();
    WriteSettings();
#if RETRO_USE_MOD_LOADER
    SaveMods();
#endif

#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_Quit();
#endif
}

bool RetroEngine::LoadGameConfig(const char *filePath)
{
    FileInfo info;
    byte fileBuffer  = 0;
    byte fileBuffer2 = 0;
    char data[0x40];
    char strBuffer[0x40];
    StrCopy(gameWindowText, "Retro-Engine");

    globalVariablesCount = 0;
#if RETRO_USE_MOD_LOADER
    playerCount = 0;
#endif

    if (LoadFile(filePath, &info)) {
        FileRead(&fileBuffer, 1);
        FileRead(gameWindowText, fileBuffer);
        gameWindowText[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(&data, fileBuffer);
        data[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(gameDescriptionText, fileBuffer);
        gameDescriptionText[fileBuffer] = 0;

        byte objectCount = 0;
        FileRead(&objectCount, 1);
        for (byte o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        for (byte s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        byte varCount = 0;
        FileRead(&varCount, 1);
        globalVariablesCount = varCount;
        for (byte v = 0; v < varCount; ++v) {
            FileRead(&fileBuffer, 1);
            FileRead(&globalVariableNames[v], fileBuffer);
            globalVariableNames[v][fileBuffer] = 0;

            FileRead(&fileBuffer2, 1);
            globalVariables[v] = fileBuffer2 << 24;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 16;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 8;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2;
        }

        byte sfxCount = 0;
        FileRead(&sfxCount, 1);
        for (byte s = 0; s < sfxCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        byte plrCount = 0;
        FileRead(&plrCount, 1);
        for (byte p = 0; p < plrCount; ++p) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        for (int c = 0; c < 4; ++c) {
            int cat = c;
            if (c == 2)
                cat = 3;
            else if (c == 3)
                cat = 2;
            FileRead(&fileBuffer, 1);
            stageListCount[cat] = fileBuffer;
            for (int s = 0; s < stageListCount[cat]; ++s) {
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].folder, fileBuffer);
                stageList[cat][s].folder[fileBuffer] = 0;

                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].id, fileBuffer);
                stageList[cat][s].id[fileBuffer] = 0;

                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].name, fileBuffer);
                stageList[cat][s].name[fileBuffer] = 0;

                FileRead(&fileBuffer, 1);
                stageList[cat][s].highlighted = fileBuffer;
            }
        }

        CloseFile();
        return true;
    }

    return false;
}

void RetroEngine::Callback(int callbackID)
{
    int notifyParam1 = GetGlobalVariableByName("game.callbackParam0");
    int notifyParam2 = GetGlobalVariableByName("game.callbackParam1");
    int notifyParam3 = GetGlobalVariableByName("game.callbackParam2");

    switch (callbackID) {
        default: PrintLog("Callback: Unknown (%d)", callbackID); break;
        case CALLBACK_RESTART_SELECTED:
            stageMode = STAGEMODE_LOAD;
            break;
        case CALLBACK_EXIT_SELECTED:
            running = false;
            break;
    }
}
