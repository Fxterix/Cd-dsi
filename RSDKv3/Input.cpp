#include "RetroEngine.hpp"
#include <nds.h>

#include <algorithm>
#include <vector>

InputData keyPress = InputData();
InputData keyDown  = InputData();

bool anyPress = false;

// Variables táctiles vacías para evitar errores de compilación en otras partes del motor
int touchDown[8] = {0};
int touchX[8] = {0};
int touchY[8] = {0};
int touchID[8] = {0};
int touches = 0;

int hapticEffectNum = -2;

#if !RETRO_USE_ORIGINAL_CODE
InputButton inputDevice[INPUT_BUTTONCOUNT];
int inputType = 0;

void ControllerInit(byte controllerID) {}
void ControllerClose(byte controllerID) {}

void ProcessInput()
{
    // Escanear los botones físicos de la DSi
    scanKeys();
    u32 keysHeld = keysHeld();
    u32 keysPressed = keysDown();

    // --- MAPEO DE CONTROLES (SIN TÁCTIL, SIN L/R) ---
    // Cruceta (Movimiento, agacharse y mirar arriba)
    inputDevice[INPUT_UP].hold    = (keysHeld & KEY_UP) != 0;
    inputDevice[INPUT_DOWN].hold  = (keysHeld & KEY_DOWN) != 0;
    inputDevice[INPUT_LEFT].hold  = (keysHeld & KEY_LEFT) != 0;
    inputDevice[INPUT_RIGHT].hold = (keysHeld & KEY_RIGHT) != 0;

    // Botones de acción principales (A y B para saltar / spindash)
    inputDevice[INPUT_BUTTONA].hold = (keysHeld & KEY_A) != 0;
    inputDevice[INPUT_BUTTONB].hold = (keysHeld & KEY_B) != 0;
    
    // Opcional: Usamos X o Y como botones adicionales de salto si los necesitas
    inputDevice[INPUT_BUTTONC].hold = (keysHeld & KEY_X) != 0; 

    // Botón Start para pausar el juego
    inputDevice[INPUT_START].hold   = (keysHeld & KEY_START) != 0;

    // Actualizar eventos de presionado único (Press)
    inputDevice[INPUT_UP].press    = (keysPressed & KEY_UP) != 0;
    inputDevice[INPUT_DOWN].press  = (keysPressed & KEY_DOWN) != 0;
    inputDevice[INPUT_LEFT].press  = (keysPressed & KEY_LEFT) != 0;
    inputDevice[INPUT_RIGHT].press = (keysPressed & KEY_RIGHT) != 0;
    inputDevice[INPUT_BUTTONA].press = (keysPressed & KEY_A) != 0;
    inputDevice[INPUT_BUTTONB].press = (keysPressed & KEY_B) != 0;
    inputDevice[INPUT_BUTTONC].press = (keysPressed & KEY_X) != 0;
    inputDevice[INPUT_START].press   = (keysPressed & KEY_START) != 0;

    // Forzar que el motor detecte pulsaciones solo con botones (sin táctil)
    bool isAnyPressed = (keysPressed != 0);
    if (isAnyPressed) {
        Engine.dimTimer = 0;
        inputDevice[INPUT_ANY].setHeld();
    } else {
        inputDevice[INPUT_ANY].setReleased();
        if (Engine.dimTimer < Engine.dimLimit && !Engine.masterPaused) {
            ++Engine.dimTimer;
        }
    }
}
#endif //! !RETRO_USE_ORIGINAL_CODE

void CheckKeyPress(InputData *input, byte flags)
{
    if (flags & 0x1)
        input->up = inputDevice[INPUT_UP].press;
    if (flags & 0x2)
        input->down = inputDevice[INPUT_DOWN].press;
    if (flags & 0x4)
        input->left = inputDevice[INPUT_LEFT].press;
    if (flags & 0x8)
        input->right = inputDevice[INPUT_RIGHT].press;
    if (flags & 0x10)
        input->A = inputDevice[INPUT_BUTTONA].press;
    if (flags & 0x20)
        input->B = inputDevice[INPUT_BUTTONB].press;
    if (flags & 0x40)
        input->C = inputDevice[INPUT_BUTTONC].press;
    if (flags & 0x80)
        input->start = inputDevice[INPUT_START].press;
    if (flags & 0x80) {
        anyPress = inputDevice[INPUT_ANY].press;
        SetGlobalVariableByName("input.pressButton", anyPress);
    }
}

void CheckKeyDown(InputData *input, byte flags)
{
    if (flags & 0x1)
        input->up = inputDevice[INPUT_UP].hold;
    if (flags & 0x2)
        input->down = inputDevice[INPUT_DOWN].hold;
    if (flags & 0x4)
        input->left = inputDevice[INPUT_LEFT].hold;
    if (flags & 0x8)
        input->right = inputDevice[INPUT_RIGHT].hold;
    if (flags & 0x10)
        input->A = inputDevice[INPUT_BUTTONA].hold;
    if (flags & 0x20)
        input->B = inputDevice[INPUT_BUTTONB].hold;
    if (flags & 0x40)
        input->C = inputDevice[INPUT_BUTTONC].hold;
    if (flags & 0x80)
        input->start = inputDevice[INPUT_START].hold;
}

#if RETRO_USE_HAPTICS
void QueueHapticEffect(int hapticID) {}
void PlayHaptics(int left, int right, int power) {}
void PlayHapticsID(int hapticID) {}
void StopHaptics(int hapticID) {}
#endif
gs & 0x4)
        input->left = inputDevice[INPUT_LEFT].hold;
    if (flags & 0x8)
        input->right = inputDevice[INPUT_RIGHT].hold;
    if (flags & 0x10)
        input->A = inputDevice[INPUT_BUTTONA].hold;
    if (flags & 0x20)
        input->B = inputDevice[INPUT_BUTTONB].hold;
    if (flags & 0x40)
        input->C = inputDevice[INPUT_BUTTONC].hold;
    if (flags & 0x80)
        input->start = inputDevice[INPUT_START].hold;
}

#if RETRO_USE_HAPTICS
void QueueHapticEffect(int hapticID) {}
void PlayHaptics(int left, int right, int power) {}
void PlayHapticsID(int hapticID) {}
void StopHaptics(int hapticID) {}
#endif
          = 103,
    HAPTIC_TEXTURE3                       = 104,
    HAPTIC_TEXTURE4                       = 105,
    HAPTIC_TEXTURE5                       = 106,
    HAPTIC_TEXTURE6                       = 107,
    HAPTIC_TEXTURE7                       = 108,
    HAPTIC_TEXTURE8                       = 109,
    HAPTIC_TEXTURE9                       = 110,
    HAPTIC_TICK_100                       = 24,
    HAPTIC_TICK_33                        = 26,
    HAPTIC_TICK_66                        = 25,
    HAPTIC_TRANSITION_BOUNCE_100          = 60,
    HAPTIC_TRANSITION_BOUNCE_33           = 62,
    HAPTIC_TRANSITION_BOUNCE_66           = 61,
    HAPTIC_TRANSITION_BUMP_100            = 57,
    HAPTIC_TRANSITION_BUMP_33             = 59,
    HAPTIC_TRANSITION_BUMP_66             = 58,
    HAPTIC_TRIPLE_STRONG_CLICK_100        = 21,
    HAPTIC_TRIPLE_STRONG_CLICK_33         = 23,
    HAPTIC_TRIPLE_STRONG_CLICK_66         = 22,
    HAPTIC_WEAPON1                        = 83,
    HAPTIC_WEAPON10                       = 92,
    HAPTIC_WEAPON2                        = 84,
    HAPTIC_WEAPON3                        = 85,
    HAPTIC_WEAPON4                        = 86,
    HAPTIC_WEAPON5                        = 87,
    HAPTIC_WEAPON6                        = 88,
    HAPTIC_WEAPON7                        = 89,
    HAPTIC_WEAPON8                        = 90,
    HAPTIC_WEAPON9                        = 91,
};

#if !RETRO_USE_ORIGINAL_CODE
#include <algorithm>
#include <vector>

InputButton inputDevice[INPUT_BUTTONCOUNT];
int inputType = 0;

// mania deadzone vals lol
float LSTICK_DEADZONE   = 0.3;
float RSTICK_DEADZONE   = 0.3;
float LTRIGGER_DEADZONE = 0.3;
float RTRIGGER_DEADZONE = 0.3;

int mouseHideTimer = 0;
int lastMouseX     = 0;
int lastMouseY     = 0;

#if RETRO_USING_SDL2
std::vector<SDL_GameController *> controllers;
#endif

#if RETRO_USING_SDL1
byte keyState[SDLK_LAST];

SDL_Joystick *controller = nullptr;
#endif

#define normalize(val, minVal, maxVal) ((float)(val) - (float)(minVal)) / ((float)(maxVal) - (float)(minVal))

#if RETRO_USING_SDL2
bool getControllerButton(byte buttonID)
{
    bool pressed = false;

    for (int i = 0; i < controllers.size(); ++i) {
        SDL_GameController *controller = controllers[i];

        if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)buttonID)) {
            pressed |= true;
            continue;
        }
        else {
            switch (buttonID) {
                default: break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta < -LSTICK_DEADZONE;
                    continue;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta > LSTICK_DEADZONE;
                    continue;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta < -LSTICK_DEADZONE;
                    continue;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta > LSTICK_DEADZONE;
                    continue;
                }
            }
        }

        switch (buttonID) {
            default: break;
            case SDL_CONTROLLER_BUTTON_ZL: {
                float delta = normalize(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT), 0, 32767);
                pressed |= delta > LTRIGGER_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_ZR: {
                float delta = normalize(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT), 0, 32767);
                pressed |= delta > RTRIGGER_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_UP: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_DOWN: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_LEFT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_RIGHT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_UP: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -RSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_DOWN: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > RSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_LEFT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > RSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_RIGHT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -RSTICK_DEADZONE;
                continue;
            }
        }
    }

    return pressed;
}
#endif //! RETRO_USING_SDL2

void ControllerInit(byte controllerID)
{
    SDL_GameController *controller = SDL_GameControllerOpen(controllerID);
    if (controller) {
        controllers.push_back(controller);
        inputType = 1;
    }
}

void ControllerClose(byte controllerID)
{
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(controllerID);
    if (controller) {
        SDL_GameControllerClose(controller);
        controllers.erase(std::remove(controllers.begin(), controllers.end(), controller), controllers.end());
    }

    if (controllers.empty()) {
        inputType = 0;
    }
}

void ProcessInput()
{
#if RETRO_USING_SDL2
    int length           = 0;
    const byte *keyState = SDL_GetKeyboardState(&length);

    if (inputType == 0) {
        for (int i = 0; i < INPUT_ANY; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                if (!inputDevice[INPUT_ANY].hold)
                    inputDevice[INPUT_ANY].setHeld();
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }
    else if (inputType == 1) {
        for (int i = 0; i < INPUT_ANY; i++) {
            if (getControllerButton(inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                if (!inputDevice[INPUT_ANY].hold)
                    inputDevice[INPUT_ANY].setHeld();
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
        if (keyState[inputDevice[i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 0;
    else if (inputType == 0)
        inputDevice[INPUT_ANY].setReleased();

    isPressed = false;
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (getControllerButton(i)) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 1;
    else if (inputType == 1)
        inputDevice[INPUT_ANY].setReleased();

    if (inputDevice[INPUT_ANY].press || inputDevice[INPUT_ANY].hold || touches > 1) {
        Engine.dimTimer = 0;
    }
    else if (Engine.dimTimer < Engine.dimLimit && !Engine.masterPaused) {
        ++Engine.dimTimer;
    }

#ifdef RETRO_USING_MOUSE
    if (touches <= 0) {
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);

        if (mx == lastMouseX && my == lastMouseY) {
            ++mouseHideTimer;
            if (mouseHideTimer == 120) {
                SDL_ShowCursor(false);
            }
        }
        else {
            if (mouseHideTimer >= 120)
                SDL_ShowCursor(true);
            mouseHideTimer  = 0;
            Engine.dimTimer = 0;
        }

        lastMouseX = mx;
        lastMouseY = my;
    }
#endif //! RETRO_USING_MOUSE

#elif RETRO_USING_SDL1
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_JoystickOpen(0);

        // There's a problem opening the joystick
        if (controller == NULL) {
            // Uh oh
        }
        else {
            inputType = 1;
        }
    }
    else {
        if (controller) {
            // Close the joystick
            SDL_JoystickClose(controller);
        }
        controller = nullptr;
        inputType  = 0;
    }

    if (inputType == 0) {
        for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }
    else if (inputType == 1 && controller) {
        for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
            if (SDL_JoystickGetButton(controller, inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            }
            else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_BUTTONCOUNT; i++) {
        if (keyState[inputDevice[i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 0;
    else if (inputType == 0)
        inputDevice[INPUT_ANY].setReleased();

    int buttonCnt = 0;
    if (controller)
        buttonCnt = SDL_JoystickNumButtons(controller);
    bool flag = false;
    for (int i = 0; i < buttonCnt; ++i) {
        flag      = true;
        inputType = 1;
    }
    if (!flag && inputType == 1) {
        inputDevice[INPUT_ANY].setReleased();
    }
#endif //! RETRO_USING_SDL2
}
#endif //! !RETRO_USE_ORIGINAL_CODE

void CheckKeyPress(InputData *input, byte flags)
{
    if (flags & 0x1)
        input->up = inputDevice[INPUT_UP].press;
    if (flags & 0x2)
        input->down = inputDevice[INPUT_DOWN].press;
    if (flags & 0x4)
        input->left = inputDevice[INPUT_LEFT].press;
    if (flags & 0x8)
        input->right = inputDevice[INPUT_RIGHT].press;
    if (flags & 0x10)
        input->A = inputDevice[INPUT_BUTTONA].press;
    if (flags & 0x20)
        input->B = inputDevice[INPUT_BUTTONB].press;
    if (flags & 0x40)
        input->C = inputDevice[INPUT_BUTTONC].press;
    if (flags & 0x80)
        input->start = inputDevice[INPUT_START].press;
    if (flags & 0x80) {
        anyPress = inputDevice[INPUT_ANY].press;
        if (!anyPress) {
            for (int t = 0; t < touches; ++t) {
                if (touchDown[t])
                    anyPress = true;
            }
        }

        SetGlobalVariableByName("input.pressButton", anyPress);
    }
}

void CheckKeyDown(InputData *input, byte flags)
{
    if (flags & 0x1)
        input->up = inputDevice[INPUT_UP].hold;
    if (flags & 0x2)
        input->down = inputDevice[INPUT_DOWN].hold;
    if (flags & 0x4)
        input->left = inputDevice[INPUT_LEFT].hold;
    if (flags & 0x8)
        input->right = inputDevice[INPUT_RIGHT].hold;
    if (flags & 0x10)
        input->A = inputDevice[INPUT_BUTTONA].hold;
    if (flags & 0x20)
        input->B = inputDevice[INPUT_BUTTONB].hold;
    if (flags & 0x40)
        input->C = inputDevice[INPUT_BUTTONC].hold;
    if (flags & 0x80)
        input->start = inputDevice[INPUT_START].hold;
}

#if RETRO_USE_HAPTICS
void QueueHapticEffect(int hapticID)
{
    if (Engine.hapticsEnabled) {
        // Haptic ID seems to be the ID for "Universal Haptic Layer"'s haptic effect library
        hapticEffectNum = hapticID;
    }
}

void PlayHaptics(int left, int right, int power) {}
void PlayHapticsID(int hapticID) {}
void StopHaptics(int hapticID) {}
#endif
