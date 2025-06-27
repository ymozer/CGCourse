#include "Input.hpp"
#include <cstring> // For memcpy/memset

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h> // Use the SDL3 backend for ImGui

namespace Base {

// Initialize static member variables
bool Input::s_Keys[SDL_SCANCODE_COUNT];
bool Input::s_KeysLastFrame[SDL_SCANCODE_COUNT];
bool Input::s_MouseButtons[Input::MAX_BUTTONS];
bool Input::s_MouseButtonsLastFrame[Input::MAX_BUTTONS];
float Input::s_MouseX = 0.0f;
float Input::s_MouseY = 0.0f;
float Input::s_MouseDeltaX = 0.0f;
float Input::s_MouseDeltaY = 0.0f;

void Input::init() {
    // Set all states to false initially
    memset(s_Keys, false, sizeof(s_Keys));
    memset(s_KeysLastFrame, false, sizeof(s_KeysLastFrame));
    memset(s_MouseButtons, false, sizeof(s_MouseButtons));
    memset(s_MouseButtonsLastFrame, false, sizeof(s_MouseButtonsLastFrame));
}

void Input::update() {
    // This function should be called at the beginning of a new frame,
    // before processing new events.

    // Copy current state to last frame's state for 'triggered'/'released' checks
    memcpy(s_KeysLastFrame, s_Keys, sizeof(s_Keys));
    memcpy(s_MouseButtonsLastFrame, s_MouseButtons, sizeof(s_MouseButtons));

    // Reset mouse delta. It will be accumulated by processEvent for the new frame.
    s_MouseDeltaX = 0.0f;
    s_MouseDeltaY = 0.0f;
}

void Input::processEvent(const SDL_Event* event) {
    // First, let ImGui process the event
    ImGui_ImplSDL3_ProcessEvent(event);

    switch (event->type) {
        case SDL_EVENT_KEY_DOWN: {
            // Do not process app input if ImGui has captured the keyboard
            if (ImGui::GetIO().WantCaptureKeyboard) break;
            
            SDL_Scancode scancode = event->key.scancode;
            if (scancode < SDL_SCANCODE_COUNT) {
                s_Keys[scancode] = true;
            }
            break;
        }
        case SDL_EVENT_KEY_UP: {
            // Do not process app input if ImGui has captured the keyboard
            if (ImGui::GetIO().WantCaptureKeyboard) break;

            SDL_Scancode scancode = event->key.scancode;
            if (scancode < SDL_SCANCODE_COUNT) {
                s_Keys[scancode] = false;
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            // Do not process app input if ImGui has captured the mouse
            if (ImGui::GetIO().WantCaptureMouse) break;

            int button = event->button.button;
            if (button < MAX_BUTTONS) {
                s_MouseButtons[button] = true;
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            // Do not process app input if ImGui has captured the mouse
            if (ImGui::GetIO().WantCaptureMouse) break;

            int button = event->button.button;
            if (button < MAX_BUTTONS) {
                s_MouseButtons[button] = false;
            }
            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            s_MouseX = event->motion.x;
            s_MouseY = event->motion.y;
            
            // Accumulate delta, as multiple motion events can occur per frame.
            s_MouseDeltaX += event->motion.xrel;
            s_MouseDeltaY += event->motion.yrel; // SDL's Y-axis is inverted from typical GL
            break;
        }
    }
}


// --- Public API Implementations ---

bool Input::isKeyPressed(SDL_Scancode key) {
    if (key >= SDL_SCANCODE_COUNT) return false;
    return s_Keys[key];
}

bool Input::isKeyTriggered(SDL_Scancode key) {
    if (key >= SDL_SCANCODE_COUNT) return false;
    return s_Keys[key] && !s_KeysLastFrame[key];
}

bool Input::isKeyReleased(SDL_Scancode key) {
    if (key >= SDL_SCANCODE_COUNT) return false;
    return !s_Keys[key] && s_KeysLastFrame[key];
}

bool Input::isMouseButtonPressed(int button) {
    if (button < 0 || button >= MAX_BUTTONS) return false;
    return s_MouseButtons[button];
}

bool Input::isMouseButtonTriggered(int button) {
    if (button < 0 || button >= MAX_BUTTONS) return false;
    return s_MouseButtons[button] && !s_MouseButtonsLastFrame[button];
}

bool Input::isMouseButtonReleased(int button) {
    if (button < 0 || button >= MAX_BUTTONS) return false;
    return !s_MouseButtons[button] && s_MouseButtonsLastFrame[button];
}

float Input::getMouseX() { return s_MouseX; }
float Input::getMouseY() { return s_MouseY; }
void Input::getMousePosition(float& x, float& y) { x = s_MouseX; y = s_MouseY; }
float Input::getMouseDeltaX() { return s_MouseDeltaX; }
float Input::getMouseDeltaY() { return s_MouseDeltaY; }

} // namespace Base