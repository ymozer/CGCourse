#pragma once

// Include the SDL header to get access to SDL_Scancode and SDL_Event
#include <SDL3/SDL.h>

// Forward declare SDL_Window (though not strictly needed by this class anymore,
// it's good practice if other parts of your engine expect it).
struct SDL_Window;

namespace Base {

class Input {
public:
    // This class is static, so it cannot be instantiated.
    Input() = delete;

    // --- Public API for Chapters ---
    // The public-facing API remains consistent.

    // Keyboard state checks (now using SDL_Scancode for clarity and robustness)
    static bool isKeyPressed(SDL_Scancode key);
    static bool isKeyTriggered(SDL_Scancode key);
    static bool isKeyReleased(SDL_Scancode key);

    // Mouse button state checks (SDL button constants are simple integers)
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonTriggered(int button);
    static bool isMouseButtonReleased(int button);

    // Mouse position and delta
    static float getMouseX();
    static float getMouseY();
    static void getMousePosition(float& x, float& y);
    static float getMouseDeltaX();
    static float getMouseDeltaY();

    // --- Internal methods called by Application ---

    // Initializes the input system's internal state.
    static void init();

    // Updates the internal state for the next frame.
    // This MUST be called once at the start or end of each frame.
    static void update();

    // Processes a single SDL event from the event queue.
    // This MUST be called for each event polled in the main loop.
    static void processEvent(const SDL_Event* event);

private:
    // Maximum number of mouse buttons supported.
    // SDL supports up to 32.
    static constexpr int MAX_BUTTONS = 32;

    // State storage
    // Using SDL_SCANCODE_COUNT is the correct way to size the key array.
    static bool s_Keys[SDL_SCANCODE_COUNT];
    static bool s_KeysLastFrame[SDL_SCANCODE_COUNT];

    static bool s_MouseButtons[MAX_BUTTONS];
    static bool s_MouseButtonsLastFrame[MAX_BUTTONS];

    static float s_MouseX, s_MouseY;
    static float s_MouseDeltaX, s_MouseDeltaY;
};

} // namespace Base