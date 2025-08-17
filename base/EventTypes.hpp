#pragma once
#include <string>
#include <memory>
#include <SDL3/SDL.h>

#include "Log.hpp"

namespace Base
{
    struct Event
    {
        std::atomic<bool> handled = {false};
        Event() = default;
        virtual ~Event() = default;
        Event(const Event &) = delete;
        Event &operator=(const Event &) = delete;
        Event(Event &&other) noexcept : handled(other.handled.load(std::memory_order_relaxed)) {}
        Event &operator=(Event &&other) noexcept
        {
            if (this != &other)
            {
                handled.store(other.handled.load(std::memory_order_relaxed), std::memory_order_relaxed);
            }
            return *this;
        }

        virtual const char *GetName() const = 0;
        virtual std::string ToString() const { return GetName(); }
    };

// Macro to simplify implementing GetName() and potentially type IDs
#define EVENT_CLASS_TYPE(type)                           \
    static const char *GetStaticName() { return #type; } \
    virtual const char *GetName() const override { return GetStaticName(); }

    struct WindowCloseEvent : public Event
    {
        Uint32 sdlWindowId; // Identify which window
        WindowCloseEvent(int id) : sdlWindowId(id)
        {
            LOG_TRACE("WindowCloseEvent created: {{id={}}}", sdlWindowId);
        }
        EVENT_CLASS_TYPE(WindowCloseEvent)
        std::string ToString() const override
        {
            return std::string("WindowCloseEvent: {id=") + std::to_string(sdlWindowId) + "}";
        }
    };

    struct WindowCreatedEvent : public Event
    {
        Uint32 sdlWindowId; // Identify which window
        WindowCreatedEvent(int id) : sdlWindowId(id)
        {
            LOG_TRACE("WindowCreatedEvent created: {}", sdlWindowId);
        }
        EVENT_CLASS_TYPE(WindowCreatedEvent)
        std::string ToString() const override
        {
            return std::string("WindowCreatedEvent: {id=") + std::to_string(sdlWindowId) + "}";
        }
    };

    struct WindowResizeEvent : public Event
    {
        Uint8 sdlWindowId;
        unsigned int width, height;
        WindowResizeEvent(int id, unsigned int w, unsigned int h) : sdlWindowId(id), width(w), height(h)
        {
        }
        EVENT_CLASS_TYPE(WindowResizeEvent)
        std::string ToString() const override
        {
            return std::string("WindowResizeEvent: {id=") + std::to_string(sdlWindowId) +
                   ", width=" + std::to_string(width) + ", height=" + std::to_string(height) + "}";
        }
    };

    struct WindowMinimizeEvent : public Event
    {
        Uint8 sdlWindowId;
        WindowMinimizeEvent(int id) : sdlWindowId(id) {}
        EVENT_CLASS_TYPE(WindowMinimizeEvent)
    };

    struct WindowMaximizeEvent : public Event
    {
        Uint8 sdlWindowId;
        WindowMaximizeEvent(int id) : sdlWindowId(id) {}
        EVENT_CLASS_TYPE(WindowMaximizeEvent)
    };

    struct WindowRestoreEvent : public Event
    {
        Uint8 sdlWindowId;
        WindowRestoreEvent(int id) : sdlWindowId(id) {}
        EVENT_CLASS_TYPE(WindowRestoreEvent)
    };

    struct WindowFocusEvent : public Event
    {
        Uint8 sdlWindowId;
        WindowFocusEvent(int id) : sdlWindowId(id) {}
        EVENT_CLASS_TYPE(WindowFocusEvent)
    };

    struct WindowLostFocusEvent : public Event
    {
        Uint8 sdlWindowId;
        WindowLostFocusEvent(int id) : sdlWindowId(id) {}
        EVENT_CLASS_TYPE(WindowLostFocusEvent)
    };

    struct ApplicationQuitEvent : public Event
    {
        ApplicationQuitEvent()
        {
        }

        EVENT_CLASS_TYPE(ApplicationQuitEvent)
    };

    struct KeyPressedEvent : public Event
    {
        SDL_Scancode scancode;
        SDL_Keycode key;
        Uint16 mod;
        Uint32 windowID;
        bool isRepeat;

        KeyPressedEvent(SDL_Scancode sc, SDL_Keycode k, Uint16 m, Uint32 wID, bool repeat)
            : scancode(sc), key(k), mod(m), windowID(wID), isRepeat(repeat) {}

        EVENT_CLASS_TYPE(KeyPressedEvent)

        std::string ToString() const override
        {
            const char* keyName = SDL_GetKeyName(key);
            return fmt::format("KeyPressedEvent: {{ keyName='{}', keyCode={}, scancode={} {} }}",
                               keyName, 
                               static_cast<int>(key), 
                               static_cast<int>(scancode),
                               isRepeat ? " (repeat)" : "");
        }
    };

    struct KeyReleasedEvent : public Event
    {
        SDL_Scancode scancode; // Physical key location
        SDL_Keycode sym;       // Key symbol (layout dependent)
        Uint16 mod;            // Modifier keys (Shift, Ctrl, Alt)
        Uint32 windowID;       // Window that had focus

        KeyReleasedEvent(SDL_Scancode sc, SDL_Keycode k, Uint16 m, Uint32 wID)
            : scancode(sc), sym(k), mod(m), windowID(wID) {}

        EVENT_CLASS_TYPE(KeyReleasedEvent)
        std::string ToString() const override
        {
            return std::string("KeyReleasedEvent: {scancode=") + std::to_string(scancode) + ", key=" + std::to_string(sym) + "}";
        }
    };

    struct MouseButtonPressedEvent : public Event
    {
        int button;      // Button index (1=left, 2=middle, 3=right, etc.)
        int clicks;      // 1 for single-click, 2 for double-click, etc.
        float x, y;      // Position where event occurred
        Uint32 windowID; // Window where event occurred

        MouseButtonPressedEvent(int btn, int clk, float mx, float my, Uint32 wID)
            : button(btn), clicks(clk), x(mx), y(my), windowID(wID)
        {
            LOG_TRACE("MouseButtonPressedEvent created: {{button={}, clicks={}, x={}, y={}}}", button, clicks, x, y);
        }

        EVENT_CLASS_TYPE(MouseButtonPressedEvent)
        std::string ToString() const override
        {
            return std::string("MouseButtonPressedEvent: {button=") + std::to_string(button) + ", clicks=" + std::to_string(clicks) + ", x=" + std::to_string(x) + ", y=" + std::to_string(y) + "}";
        }
    };

    struct MouseButtonReleasedEvent : public Event
    {
        int button;      // Button index
        float x, y;      // Position where event occurred
        Uint32 windowID; // Window where event occurred

        MouseButtonReleasedEvent(int btn, float mx, float my, Uint32 wID)
            : button(btn), x(mx), y(my), windowID(wID) {}

        EVENT_CLASS_TYPE(MouseButtonReleasedEvent)
        std::string ToString() const override
        {
            return std::string("MouseButtonReleasedEvent: {button=") + std::to_string(button) + ", x=" + std::to_string(x) + ", y=" + std::to_string(y) + "}";
        }
    };

    struct MouseMovedEvent : public Event
    {
        float x, y;       // Current absolute position
        float xrel, yrel; // Relative motion since last event
        Uint32 windowID;  // Window where event occurred

        MouseMovedEvent(float mx, float my, float rx, float ry, Uint32 wID)
            : x(mx), y(my), xrel(rx), yrel(ry), windowID(wID)
        {
            // LOG_TRACE("MouseMovedEvent created: {{x={}, y={}}}", x, y);
        }

        EVENT_CLASS_TYPE(MouseMovedEvent)
        std::string ToString() const override
        {
            return std::string("MouseMovedEvent: {x=") + std::to_string(x) + ", y=" + std::to_string(y) + "}";
        }
    };

    struct MouseScrolledEvent : public Event
    {
        float x, y; // Scroll amount (horizontal/vertical)
        Uint32 windowID;

        MouseScrolledEvent(float sx, float sy, Uint32 wID)
            : x(sx), y(sy), windowID(wID) {}

        EVENT_CLASS_TYPE(MouseScrolledEvent)
        std::string ToString() const override
        {
            return std::string("MouseScrolledEvent: {x=") + std::to_string(x) + ", y=" + std::to_string(y) + "}";
        }
    };

    struct GamepadConnectedEvent : public Event
    {
        SDL_JoystickID which; // Instance ID of the connected gamepad
        std::string name;

        GamepadConnectedEvent(SDL_JoystickID id, const char *n) : which(id), name(n ? n : "") {}
        EVENT_CLASS_TYPE(GamepadConnectedEvent)
        std::string ToString() const override { return "GamepadConnectedEvent: {id=" + std::to_string(which) + ", name='" + name + "'}"; }
    };

    struct GamepadDisconnectedEvent : public Event
    {
        SDL_JoystickID which; // Instance ID of the disconnected gamepad

        GamepadDisconnectedEvent(SDL_JoystickID id) : which(id) {}
        EVENT_CLASS_TYPE(GamepadDisconnectedEvent)
        std::string ToString() const override { return "GamepadDisconnectedEvent: {id=" + std::to_string(which) + "}"; }
    };

    struct GamepadButtonPressedEvent : public Event
    {
        SDL_JoystickID which; // Instance ID
        Uint8 button;         // Which button

        GamepadButtonPressedEvent(SDL_JoystickID id, Uint8 btn) : which(id), button(btn) {}
        EVENT_CLASS_TYPE(GamepadButtonPressedEvent)
        std::string ToString() const override { return "GamepadButtonPressedEvent: {id=" + std::to_string(which) + ", button=" + std::to_string(button) + "}"; }
    };

    struct GamepadButtonReleasedEvent : public Event
    {
        SDL_JoystickID which; // Instance ID
        Uint8 button;         // Which button

        GamepadButtonReleasedEvent(SDL_JoystickID id, Uint8 btn) : which(id), button(btn) {}
        EVENT_CLASS_TYPE(GamepadButtonReleasedEvent)
        std::string ToString() const override { return "GamepadButtonReleasedEvent: {id=" + std::to_string(which) + ", button=" + std::to_string(button) + "}"; }
    };

    struct GamepadAxisMovedEvent : public Event
    {
        SDL_JoystickID which;  // Instance ID
        Uint8 axis;            // Which axis
        Sint16 rawValue;       // Raw value from SDL (-32768 to 32767)
        float normalizedValue; // Value after deadzone (-1.0 to 1.0)

        GamepadAxisMovedEvent(SDL_JoystickID id, Uint8 ax, Sint16 raw, float norm)
            : which(id), axis(ax), rawValue(raw), normalizedValue(norm) {}
        EVENT_CLASS_TYPE(GamepadAxisMovedEvent)
        std::string ToString() const override { return "GamepadAxisMovedEvent: {id=" + std::to_string(which) + ", axis=" + std::to_string(axis) + ", value=" + std::to_string(normalizedValue) + "}"; }
    };

    struct FingerDownEvent : public Event
    {
        EVENT_CLASS_TYPE(FingerDown) // Assumes you have a macro/enum for types

        SDL_TouchID touchId;
        SDL_FingerID fingerId;
        float x;        // Normalized (0..1)
        float y;        // Normalized (0..1)
        float pressure; // Normalized (0..1)
        // Uint32 windowID; // Optional

        FingerDownEvent(SDL_TouchID tid, SDL_FingerID fid, float nx, float ny, float np /*, Uint32 wid = 0*/)
            : touchId(tid), fingerId(fid), x(nx), y(ny), pressure(np) /*, windowID(wid)*/ {}

        // Optional: toString() for logging
        std::string ToString() const override
        {
            return fmt::format("FingerDownEvent: Device={}, Finger={}, Pos=({:.3f}, {:.3f}), Pressure={:.3f}",
                               touchId, fingerId, x, y, pressure);
        }
    };

    struct FingerUpEvent : public Event
    {
        EVENT_CLASS_TYPE(FingerUp)

        SDL_TouchID touchId;
        SDL_FingerID fingerId;
        float x;        // Final normalized X
        float y;        // Final normalized Y
        float pressure; // Final pressure (often 0)
        // Uint32 windowID; // Optional

        FingerUpEvent(SDL_TouchID tid, SDL_FingerID fid, float nx, float ny, float np /*, Uint32 wid = 0*/)
            : touchId(tid), fingerId(fid), x(nx), y(ny), pressure(np) /*, windowID(wid)*/ {}

        std::string ToString() const override
        {
            return fmt::format("FingerUpEvent: Device={}, Finger={}, Pos=({:.3f}, {:.3f}), Pressure={:.3f}",
                               touchId, fingerId, x, y, pressure);
        }
    };

    struct FingerMotionEvent : public Event
    {
        EVENT_CLASS_TYPE(FingerMotion)

        SDL_TouchID touchId;
        SDL_FingerID fingerId;
        float x;        // Current normalized X
        float y;        // Current normalized Y
        float dx;       // Delta X since last event (normalized)
        float dy;       // Delta Y since last event (normalized)
        float pressure; // Current pressure
        // Uint32 windowID; // Optional

        FingerMotionEvent(SDL_TouchID tid, SDL_FingerID fid, float nx, float ny, float ndx, float ndy, float np /*, Uint32 wid = 0*/)
            : touchId(tid), fingerId(fid), x(nx), y(ny), dx(ndx), dy(ndy), pressure(np) /*, windowID(wid)*/ {}

        std::string ToString() const override
        {
            return fmt::format("FingerMotionEvent: Device={}, Finger={}, Pos=({:.3f}, {:.3f}), Delta=({:.3f}, {:.3f}), Pressure={:.3f}",
                               touchId, fingerId, x, y, dx, dy, pressure);
        }
    };

}