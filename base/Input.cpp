#include "Input.hpp"
#include "Log.hpp"
#include "EventBus.hpp"
#include "EventTypes.hpp"

#include <cstring>
#include <cmath>
#include <limits>
#include <vector>
#include <glm/glm.hpp>
#include <backends/imgui_impl_sdl3.h>

namespace Base
{
    struct ButtonState
    {
        bool current = false;
        bool previous = false;
    };

    struct AxisState
    {
        Sint16 currentRaw = 0;
        Sint16 previousRaw = 0;
        float currentNormalized = 0.0f;
    };

    struct FingerState
    {
        SDL_FingerID id = 0;
        glm::vec2 currentPos = {0.0f, 0.0f};
        glm::vec2 previousPos = {0.0f, 0.0f};
        float currentPressure = 0.0f;
        float previousPressure = 0.0f;
        bool isDown = false;
        bool pressedThisFrame = false;
        bool releasedThisFrame = false;
    };

    struct TouchDeviceState
    {
        SDL_TouchID id = 0;
        SDL_TouchDeviceType type = SDL_TOUCH_DEVICE_INVALID;
        std::string name; // Store the name
        std::unordered_map<SDL_FingerID, FingerState> activeFingers;

        TouchDeviceState(SDL_TouchID touchID) : id(touchID)
        {
            type = SDL_GetTouchDeviceType(id);
            const char *deviceName = SDL_GetTouchDeviceName(id);
            name = deviceName ? deviceName : "Unknown Touch Device";
            LOG_INFO("Touch Device Added/Detected: ID={}, Name='{}', Type={}", id, name, (int)type);
        }
        ~TouchDeviceState()
        {
            LOG_INFO("Touch Device State Removed: ID={}", id);
        }
    };

    struct GamepadState
    {
        SDL_Gamepad *handle = nullptr;
        std::string name;
        std::vector<ButtonState> buttons;
        std::vector<AxisState> axes;

        GamepadState(SDL_Gamepad *h) : handle(h)
        {
            name = SDL_GetGamepadName(handle);
            if (name.empty())
                name = "Unknown Gamepad";
            buttons.resize(SDL_GAMEPAD_BUTTON_COUNT);
            axes.resize(SDL_GAMEPAD_AXIS_COUNT);
            LOG_INFO("Gamepad Connected: ID={}, Name='{}'", SDL_GetGamepadID(handle), name);
        }
        ~GamepadState()
        {
            if (handle)
            {
                LOG_INFO("Closing Gamepad: ID={}, Name='{}'", SDL_GetGamepadID(handle), name);
                SDL_CloseGamepad(handle);
            }
        }
    };

    struct JoystickState
    {
        SDL_Joystick *handle = nullptr;
        std::string name;
        int numAxes = 0;
        int numButtons = 0;
        int numHats = 0;
        std::vector<ButtonState> buttons;
        std::vector<AxisState> axes;
        std::vector<ButtonState> hats;

        JoystickState(SDL_Joystick *h) : handle(h)
        {
            name = SDL_GetJoystickName(handle);
            if (name.empty())
                name = "Unknown Joystick";
            numAxes = SDL_GetNumJoystickAxes(handle);
            numButtons = SDL_GetNumJoystickButtons(handle);
            numHats = SDL_GetNumJoystickHats(handle);
            buttons.resize(numButtons);
            axes.resize(numAxes);
            hats.resize(numHats * 4);
            LOG_INFO("Joystick Connected: ID={}, Name='{}' (Axes: {}, Buttons: {}, Hats: {})",
                     SDL_GetJoystickID(handle), name.c_str(), numAxes, numButtons, numHats);
        }
        ~JoystickState()
        {
            if (handle)
            {
                LOG_INFO("Closing Joystick: ID={}, Name='{}'", SDL_GetJoystickID(handle), name);
            }
        }
    };

    std::unique_ptr<Input> Input::s_InputInstance = nullptr;

    Input &Input::Get()
    {
        if (!s_InputInstance)
        {
            s_InputInstance.reset(new Input());
        }
        return *s_InputInstance;
    }

    bool Input::Initialize(SDL_Window *window, ParallelEventBus &bus, bool imguiEnabled)
    {
        if (!window)
        {
            LOG_CRITICAL("Input System initialized with a null window!");
            return false;
        }
        m_Window = window; // Store the window pointer

        m_imguiEnabled = imguiEnabled;
        LOG_INFO("Input System initialized. ImGui event processing: {}", m_imguiEnabled ? "Enabled" : "Disabled");
        m_EventBus = &bus;

        if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
        {
            return false;
        }
        m_CurrentKeyState = SDL_GetKeyboardState(&m_NumKeys);
        if (!m_CurrentKeyState || m_NumKeys == 0)
        {
            return false;
        }
        m_PreviousKeyState.resize(m_NumKeys, 0);
        memcpy(m_PreviousKeyState.data(), m_CurrentKeyState, m_NumKeys * sizeof(uint8_t));
        m_CurrentMouseButtonState = SDL_GetMouseState(&m_CurrentMousePos.x, &m_CurrentMousePos.y);
        m_PreviousMouseButtonState = m_CurrentMouseButtonState;
        m_PreviousMousePos = m_CurrentMousePos;

        int numJoysticks = 0;
        SDL_JoystickID *joysticks = SDL_GetJoysticks(&numJoysticks);
        if (joysticks)
        {
            LOG_DEBUG("Found {} joysticks already connected during init.", numJoysticks);
            for (int i = 0; i < numJoysticks; ++i)
            {
                AddDevice(joysticks[i]);
            }
            SDL_free(joysticks);
        }

        int numTouchDevices = 0;
        SDL_TouchID *touchDevices = SDL_GetTouchDevices(&numTouchDevices);
        if (touchDevices)
        {
            for (int i = 0; i < numTouchDevices; ++i)
            {
                const char *deviceName = SDL_GetTouchDeviceName(touchDevices[i]);
                if (deviceName)
                {
                    LOG_DEBUG("Touch Device Found: ID={}, Name='{}'", touchDevices[i], deviceName);
                }
                else
                {
                    LOG_DEBUG("Touch Device Found: ID={}, Name='Unknown Touch Device'", touchDevices[i]);
                }
            }
            LOG_DEBUG("Found {} touch devices detected at init.", numTouchDevices);
            SDL_free(touchDevices);
        }
        m_RelativeMouseMode = SDL_GetWindowRelativeMouseMode(m_Window);

        LOG_INFO("Input System Initialized successfully.");
        return true;
    }

    void Input::Shutdown()
    {
        LOG_INFO("Shutting Down Input System...");
        m_GamepadStates.clear();
        m_JoystickStates.clear();
        m_TouchStates.clear();

        m_CurrentKeyState = nullptr;
        m_PreviousKeyState.clear();

        m_EventBus = nullptr;

        SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD);
        LOG_INFO("Input System Shutdown complete.");
    }

    void Input::PrepareForFrame()
    {
        if (m_CurrentKeyState && !m_PreviousKeyState.empty())
        {
            memcpy(m_PreviousKeyState.data(), m_CurrentKeyState, m_NumKeys * sizeof(Uint8));
        }

        m_PreviousMouseButtonState = m_CurrentMouseButtonState;
        m_PreviousMousePos = m_CurrentMousePos;

        m_MouseWheelDelta = {0.0f, 0.0f};
        m_MouseDelta = {0, 0};

        for (auto &[id, state] : m_GamepadStates)
        {
            for (size_t i = 0; i < state->buttons.size(); ++i)
            {
                state->buttons[i].previous = state->buttons[i].current;
            }
            for (size_t i = 0; i < state->axes.size(); ++i)
            {
                state->axes[i].previousRaw = state->axes[i].currentRaw;
            }
        }
        for (auto &[id, state] : m_JoystickStates)
        {
            for (size_t i = 0; i < state->buttons.size(); ++i)
            {
                state->buttons[i].previous = state->buttons[i].current;
            }
            for (size_t i = 0; i < state->axes.size(); ++i)
            {
                state->axes[i].previousRaw = state->axes[i].currentRaw;
            }
            for (size_t i = 0; i < state->hats.size(); ++i)
            {
                state->hats[i].previous = state->hats[i].current;
            }
        }

        for (auto &[touchId, deviceState] : m_TouchStates)
        {
            std::vector<SDL_FingerID> fingersToRemove;
            for (auto &[fingerId, fingerState] : deviceState->activeFingers)
            {
                if (!fingerState.isDown)
                {
                    fingersToRemove.push_back(fingerId);
                }
                else
                {
                    fingerState.previousPos = fingerState.currentPos;
                    fingerState.previousPressure = fingerState.currentPressure;
                    fingerState.pressedThisFrame = false;
                    fingerState.releasedThisFrame = false;
                }
            }
            for (SDL_FingerID fid : fingersToRemove)
            {
                deviceState->activeFingers.erase(fid);
                LOG_TRACE("Cleaned up released finger: Device={}, Finger={}", touchId, fid);
            }
        }
    }

    // TODO: Break this function into smaller functions for each input device type
    void Input::ProcessEvent(const SDL_Event &event)
    {
        if (!m_EventBus)
        {
            LOG_WARN("Input::ProcessEvent called but EventBus is not set!");
            return;
        }

        if (m_imguiEnabled)
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            // TODO: Check io.WantCaptureMouse/Keyboard here to prevent game input
            // ImGuiIO& io = ImGui::GetIO();
            // if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
            //     // Potentially mark event as handled or return early
            // }
        }

        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
        {
            bool is_a_repeat = (event.key.repeat != 0);

            KeyPressedEvent customEvent(
                event.key.scancode, event.key.key,
                event.key.mod, event.key.windowID, is_a_repeat);
            m_EventBus->dispatch(customEvent);

            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            KeyReleasedEvent customEvent(
                event.key.scancode,
                event.key.key,
                event.key.mod,
                event.key.windowID);
            m_EventBus->dispatch(customEvent);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            MouseButtonPressedEvent customEvent(
                event.button.button,
                event.button.clicks,
                event.button.x,
                event.button.y,
                event.button.windowID);
            m_EventBus->dispatch(customEvent);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            MouseButtonReleasedEvent customEvent(
                event.button.button,
                event.button.x,
                event.button.y,
                event.button.windowID);
            m_EventBus->dispatch(customEvent);
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            MouseMovedEvent customEvent(
                event.motion.x,
                event.motion.y,
                event.motion.xrel,
                event.motion.yrel,
                event.motion.windowID);
            m_EventBus->dispatch(customEvent);
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            m_MouseWheelDelta.x += event.wheel.x;
            m_MouseWheelDelta.y += event.wheel.y;
            MouseScrolledEvent customEvent(
                event.wheel.x,
                event.wheel.y,
                event.wheel.windowID);
            m_EventBus->dispatch(customEvent);
            break;
        }

        case SDL_EVENT_JOYSTICK_ADDED:
        case SDL_EVENT_GAMEPAD_ADDED:
            AddDevice(event.jdevice.which);
            break;

        case SDL_EVENT_JOYSTICK_REMOVED:
        case SDL_EVENT_GAMEPAD_REMOVED:
            RemoveDevice(event.jdevice.which);
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            if (auto it = m_GamepadStates.find(event.gbutton.which); it != m_GamepadStates.end())
            {
                if (event.gbutton.button < it->second->buttons.size())
                {
                    it->second->buttons[event.gbutton.button].current = true;
                }
            }
            {
                GamepadButtonPressedEvent customEvent(event.gbutton.which, event.gbutton.button);
                m_EventBus->dispatch(customEvent);
            }
            break;
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            if (auto it = m_GamepadStates.find(event.gbutton.which); it != m_GamepadStates.end())
            {
                if (event.gbutton.button < it->second->buttons.size())
                {
                    it->second->buttons[event.gbutton.button].current = false; // Update state
                }
            }
            {
                GamepadButtonReleasedEvent customEvent(event.gbutton.which, event.gbutton.button);
                m_EventBus->dispatch(customEvent);
            }
            break;
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        {
            float normalizedValue = 0.0f;
            if (auto it = m_GamepadStates.find(event.gaxis.which); it != m_GamepadStates.end())
            {
                if (event.gaxis.axis < it->second->axes.size())
                {
                    it->second->axes[event.gaxis.axis].currentRaw = event.gaxis.value;
                    normalizedValue = ApplyDeadzone(it->second->axes[event.gaxis.axis].currentRaw, m_GamepadAxisDeadzone);
                    it->second->axes[event.gaxis.axis].currentNormalized = normalizedValue;
                }
            }
            GamepadAxisMovedEvent customEvent(
                event.gaxis.which,
                event.gaxis.axis,
                event.gaxis.value,
                normalizedValue);
            m_EventBus->dispatch(customEvent);
            break;
        }

        // TODO: Implement Joystick Events
        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
            // Update internal state...
            // Dispatch JoystickButtonPressedEvent...
            break;
        case SDL_EVENT_JOYSTICK_BUTTON_UP:
            // Update internal state...
            // Dispatch JoystickButtonReleasedEvent...
            break;
        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            // Update internal state...
            // Dispatch JoystickAxisMovedEvent...
            break;
        case SDL_EVENT_JOYSTICK_HAT_MOTION:
            // Update internal state...
            // Dispatch JoystickHatMovedEvent...
            break;

        case SDL_EVENT_FINGER_DOWN:
        {
            SDL_TouchID touchId = event.tfinger.touchID;
            SDL_FingerID fingerId = event.tfinger.fingerID;

            if (m_TouchStates.find(touchId) == m_TouchStates.end())
            {
                m_TouchStates[touchId] = std::make_unique<TouchDeviceState>(touchId);
            }

            auto &fingerState = m_TouchStates[touchId]->activeFingers[fingerId];

            fingerState.id = fingerId;
            fingerState.currentPos = {event.tfinger.x, event.tfinger.y};
            fingerState.previousPos = fingerState.currentPos;
            fingerState.currentPressure = event.tfinger.pressure;
            fingerState.previousPressure = fingerState.currentPressure;
            fingerState.isDown = true;
            fingerState.pressedThisFrame = true;
            fingerState.releasedThisFrame = false;

            FingerDownEvent customEvent(
                touchId,
                fingerId,
                fingerState.currentPos.x,
                fingerState.currentPos.y,
                fingerState.currentPressure);
            m_EventBus->dispatch(customEvent);
            LOG_TRACE("Finger Down: Device={}, Finger={}, Pos=({:.3f}, {:.3f}), Pressure={:.3f}",
                      touchId, fingerId, fingerState.currentPos.x, fingerState.currentPos.y, fingerState.currentPressure);
            break;
        }

        case SDL_EVENT_FINGER_MOTION:
        {
            SDL_TouchID touchId = event.tfinger.touchID;
            SDL_FingerID fingerId = event.tfinger.fingerID;

            auto touchIt = m_TouchStates.find(touchId);
            if (touchIt != m_TouchStates.end())
            {
                auto fingerIt = touchIt->second->activeFingers.find(fingerId);
                if (fingerIt != touchIt->second->activeFingers.end())
                {
                    auto &fingerState = fingerIt->second;

                    fingerState.currentPos = {event.tfinger.x, event.tfinger.y};
                    fingerState.currentPressure = event.tfinger.pressure;

                    float dx = event.tfinger.dx;
                    float dy = event.tfinger.dy;

                    FingerMotionEvent customEvent(
                        touchId,
                        fingerId,
                        fingerState.currentPos.x,
                        fingerState.currentPos.y,
                        dx,
                        dy,
                        fingerState.currentPressure);
                    m_EventBus->dispatch(customEvent);
                    LOG_TRACE("Finger Motion: Device={}, Finger={}, Pos=({:.3f}, {:.3f}), Delta=({:.3f}, {:.3f}), Pressure={:.3f}",
                              touchId, fingerId, fingerState.currentPos.x, fingerState.currentPos.y, dx, dy, fingerState.currentPressure);
                }
                else
                {
                    LOG_WARN("Finger Motion event for unknown finger: Device={}, Finger={}", touchId, fingerId);
                }
            }
            else
            {
                LOG_WARN("Finger Motion event for unknown touch device: Device={}", touchId);
            }
            break;
        }

        case SDL_EVENT_FINGER_UP:
        {
            SDL_TouchID touchId = event.tfinger.touchID;
            SDL_FingerID fingerId = event.tfinger.fingerID;

            auto touchIt = m_TouchStates.find(touchId);
            if (touchIt != m_TouchStates.end())
            {
                auto fingerIt = touchIt->second->activeFingers.find(fingerId);
                if (fingerIt != touchIt->second->activeFingers.end())
                {
                    auto &fingerState = fingerIt->second;

                    fingerState.currentPos = {event.tfinger.x, event.tfinger.y};
                    fingerState.currentPressure = event.tfinger.pressure;
                    fingerState.isDown = false;
                    fingerState.releasedThisFrame = true;
                    fingerState.pressedThisFrame = false;

                    FingerUpEvent customEvent(
                        touchId,
                        fingerId,
                        fingerState.currentPos.x,
                        fingerState.currentPos.y,
                        fingerState.currentPressure);
                    m_EventBus->dispatch(customEvent);
                    LOG_TRACE("Finger Up: Device={}, Finger={}, Pos=({:.3f}, {:.3f}), Pressure={:.3f}",
                              touchId, fingerId, fingerState.currentPos.x, fingerState.currentPos.y, fingerState.currentPressure);
                }
                else
                {
                    LOG_WARN("Finger Up event for unknown finger: Device={}, Finger={}", touchId, fingerId);
                }
            }
            else
            {
                LOG_WARN("Finger Up event for unknown touch device: Device={}", touchId);
            }
            break;
        }
        default:
            break;
        }
    }

    void Input::Update()
    {
        m_CurrentKeyState = SDL_GetKeyboardState(nullptr);
        if (m_RelativeMouseMode)
        {
            float deltaX = 0.0f;
            float deltaY = 0.0f;

            m_CurrentMouseButtonState = SDL_GetRelativeMouseState(&deltaX, &deltaY);

            m_MouseDelta = {deltaX, deltaY};

            SDL_GetMouseState(&m_CurrentMousePos.x, &m_CurrentMousePos.y);
        }
        else
        {
            m_CurrentMouseButtonState = SDL_GetMouseState(&m_CurrentMousePos.x, &m_CurrentMousePos.y);
            m_MouseDelta = m_CurrentMousePos - m_PreviousMousePos;
        }

        for (auto &[id, state] : m_GamepadStates)
        {
            if (!state->handle)
                continue;
            for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i)
            {
                state->buttons[i].current = SDL_GetGamepadButton(state->handle, (SDL_GamepadButton)i);
            }
            for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; ++i)
            {
                state->axes[i].currentRaw = SDL_GetGamepadAxis(state->handle, (SDL_GamepadAxis)i);
                state->axes[i].currentNormalized = ApplyDeadzone(state->axes[i].currentRaw, m_GamepadAxisDeadzone);
            }
        }

        for (auto &[id, state] : m_JoystickStates)
        {
            if (!state->handle)
                continue;
            for (int i = 0; i < state->numButtons; ++i)
            {
                state->buttons[i].current = SDL_GetJoystickButton(state->handle, i);
            }
            for (int i = 0; i < state->numAxes; ++i)
            {
                state->axes[i].currentRaw = SDL_GetJoystickAxis(state->handle, i);
                state->axes[i].currentNormalized = ApplyDeadzone(state->axes[i].currentRaw, m_JoystickAxisDeadzone);
            }
            for (int i = 0; i < state->numHats; ++i)
            {
                Uint8 hatValue = SDL_GetJoystickHat(state->handle, i);
                for (int j = 0; j < 4; ++j)
                {
                    state->hats[i * 4 + j].current = false;
                }
                if (hatValue & SDL_HAT_UP)
                    state->hats[i * 4 + 0].current = true;
                if (hatValue & SDL_HAT_DOWN)
                    state->hats[i * 4 + 1].current = true;
                if (hatValue & SDL_HAT_LEFT)
                    state->hats[i * 4 + 2].current = true;
                if (hatValue & SDL_HAT_RIGHT)
                    state->hats[i * 4 + 3].current = true;
            }
        }
    }

    void Input::AddDevice(SDL_JoystickID which)
    {
        if (!m_EventBus)
            return;

        bool isGamepad = SDL_IsGamepad(which);
        std::string deviceName = "";

        if (isGamepad)
        {
            if (m_GamepadStates.find(which) == m_GamepadStates.end())
            {
                SDL_Gamepad *handle = SDL_OpenGamepad(which);
                if (handle)
                {
                    m_GamepadStates[which] = std::make_unique<GamepadState>(handle);
                    deviceName = SDL_GetGamepadName(handle);
                    GamepadConnectedEvent event(which, deviceName.c_str());
                    m_EventBus->dispatch(event);
                }
                else
                {
                    LOG_ERROR("Gamepad identifier not found: {}", SDL_GetError());
                }
            }
        }
        else
        {
            if (m_JoystickStates.find(which) == m_JoystickStates.end())
            {
                SDL_Joystick *handle = SDL_OpenJoystick(which);
                if (handle)
                {
                    m_JoystickStates[which] = std::make_unique<JoystickState>(handle);
                    deviceName = SDL_GetJoystickName(handle);
                    LOG_INFO("TODO: Dispatch JoystickConnectedEvent");
                }
                else
                {
                    LOG_ERROR("Joystick identifier not found: {}", SDL_GetError());
                }
            }
        }
    }

    void Input::RemoveDevice(SDL_JoystickID which)
    {
        if (!m_EventBus)
            return;

        bool gamepadRemoved = false;
        bool joystickRemoved = false;

        if (m_GamepadStates.count(which))
        {
            m_GamepadStates.erase(which);
            gamepadRemoved = true;
            GamepadDisconnectedEvent event(which);
            m_EventBus->dispatch(event);
        }
        if (m_JoystickStates.count(which))
        {
            m_JoystickStates.erase(which);
            joystickRemoved = true;
            LOG_INFO("TODO: Dispatch JoystickDisconnectedEvent");
        }

        if (!gamepadRemoved && !joystickRemoved)
        {
            LOG_WARN("Attempted to remove device ID {}, but it was not found.", which);
        }
    }

    float Input::ApplyDeadzone(Sint16 value, float deadzoneThreshold) const
    {
        constexpr float MAX_AXIS_VALUE = (float)SDL_JOYSTICK_AXIS_MAX;
        float normalized = static_cast<float>(value) / MAX_AXIS_VALUE;

        if (std::abs(normalized) < deadzoneThreshold)
        {
            return 0.0f;
        }
        else
        {
            float sign = (normalized > 0.0f) ? 1.0f : -1.0f;
            return sign * (std::abs(normalized) - deadzoneThreshold) / (1.0f - deadzoneThreshold);
        }
    }

    bool Input::IsKeyDown(SDL_Scancode scancode) const
    {
        if (!m_CurrentKeyState || scancode >= m_NumKeys)
            return false;
        return m_CurrentKeyState[scancode] != 0;
    }

    bool Input::IsKeyPressed(SDL_Scancode scancode) const
    {
        if (!m_CurrentKeyState || !m_PreviousKeyState.data() || scancode >= m_NumKeys)
            return false;
        return (m_CurrentKeyState[scancode] != 0) && (m_PreviousKeyState[scancode] == 0);
    }

    bool Input::IsKeyReleased(SDL_Scancode scancode) const
    {
        if (!m_CurrentKeyState || !m_PreviousKeyState.data() || scancode >= m_NumKeys)
            return false;
        return (m_CurrentKeyState[scancode] == 0) && (m_PreviousKeyState[scancode] != 0);
    }

    bool Input::IsMouseButtonDown(int button) const
    {
        Uint32 mask = SDL_BUTTON_MASK(button);
        return (m_CurrentMouseButtonState & mask) != 0;
    }

    bool Input::IsMouseButtonPressed(int button) const
    {
        Uint32 mask = SDL_BUTTON_MASK(button);
        return ((m_CurrentMouseButtonState & mask) != 0) && ((m_PreviousMouseButtonState & mask) == 0);
    }

    bool Input::IsMouseButtonReleased(int button) const
    {
        Uint32 mask = SDL_BUTTON_MASK(button);
        return ((m_CurrentMouseButtonState & mask) == 0) && ((m_PreviousMouseButtonState & mask) != 0);
    }

    glm::vec2 Input::GetMousePosition() const
    {
        return m_CurrentMousePos;
    }

    glm::vec2 Input::GetMouseDelta() const
    {
        return m_MouseDelta;
    }

    glm::vec2 Input::GetMouseWheel() const
    {
        return m_MouseWheelDelta;
    }

    bool Input::IsGamepadConnected(SDL_JoystickID instanceId) const
    {
        return m_GamepadStates.count(instanceId) > 0;
    }

    bool Input::IsGamepadButtonDown(SDL_JoystickID instanceId, SDL_GamepadButton button) const
    {
        auto it = m_GamepadStates.find(instanceId);
        if (it == m_GamepadStates.end() || button >= it->second->buttons.size())
            return false;
        return it->second->buttons[button].current;
    }

    bool Input::IsGamepadButtonPressed(SDL_JoystickID instanceId, SDL_GamepadButton button) const
    {
        auto it = m_GamepadStates.find(instanceId);
        if (it == m_GamepadStates.end() || button >= it->second->buttons.size())
            return false;
        return it->second->buttons[button].current && !it->second->buttons[button].previous;
    }

    bool Input::IsGamepadButtonReleased(SDL_JoystickID instanceId, SDL_GamepadButton button) const
    {
        auto it = m_GamepadStates.find(instanceId);
        if (it == m_GamepadStates.end() || button >= it->second->buttons.size())
            return false;
        return !it->second->buttons[button].current && it->second->buttons[button].previous;
    }

    float Input::GetGamepadAxis(SDL_JoystickID instanceId, SDL_GamepadAxis axis) const
    {
        auto it = m_GamepadStates.find(instanceId);
        if (it == m_GamepadStates.end() || axis >= it->second->axes.size())
            return 0.0f;
        return it->second->axes[axis].currentNormalized;
    }

    SDL_Gamepad *Input::GetGamepadHandle(SDL_JoystickID instanceId) const
    {
        auto it = m_GamepadStates.find(instanceId);
        return (it != m_GamepadStates.end()) ? it->second->handle : nullptr;
    }

    std::vector<SDL_JoystickID> Input::GetConnectedGamepadIDs() const
    {
        std::vector<SDL_JoystickID> ids;
        ids.reserve(m_GamepadStates.size());
        for (const auto &[id, state] : m_GamepadStates)
        {
            ids.push_back(id);
        }
        return ids;
    }

    bool Input::IsJoystickConnected(SDL_JoystickID instanceId) const
    {
        return m_JoystickStates.count(instanceId) > 0;
    }

    bool Input::IsJoystickButtonDown(SDL_JoystickID instanceId, int buttonIndex) const
    {
        auto it = m_JoystickStates.find(instanceId);
        if (it == m_JoystickStates.end() || buttonIndex < 0 || buttonIndex >= it->second->buttons.size())
            return false;
        return it->second->buttons[buttonIndex].current;
    }

    bool Input::IsJoystickButtonPressed(SDL_JoystickID instanceId, int buttonIndex) const
    {
        auto it = m_JoystickStates.find(instanceId);
        if (it == m_JoystickStates.end() || buttonIndex < 0 || buttonIndex >= it->second->buttons.size())
            return false;
        return it->second->buttons[buttonIndex].current && !it->second->buttons[buttonIndex].previous;
    }

    bool Input::IsJoystickButtonReleased(SDL_JoystickID instanceId, int buttonIndex) const
    {
        auto it = m_JoystickStates.find(instanceId);
        if (it == m_JoystickStates.end() || buttonIndex < 0 || buttonIndex >= it->second->buttons.size())
            return false;
        return !it->second->buttons[buttonIndex].current && it->second->buttons[buttonIndex].previous;
    }

    float Input::GetJoystickAxis(SDL_JoystickID instanceId, int axisIndex) const
    {
        auto it = m_JoystickStates.find(instanceId);
        if (it == m_JoystickStates.end() || axisIndex < 0 || axisIndex >= it->second->axes.size())
            return 0.0f;
        return it->second->axes[axisIndex].currentNormalized;
    }

    Uint8 Input::GetJoystickHat(SDL_JoystickID instanceId, int hatIndex) const
    {
        auto it = m_JoystickStates.find(instanceId);
        if (it == m_JoystickStates.end() || hatIndex < 0 || hatIndex >= it->second->numHats)
            return SDL_HAT_CENTERED;

        Uint8 value = SDL_HAT_CENTERED;
        if (it->second->hats[hatIndex * 4 + 0].current)
            value |= SDL_HAT_UP;
        if (it->second->hats[hatIndex * 4 + 1].current)
            value |= SDL_HAT_DOWN;
        if (it->second->hats[hatIndex * 4 + 2].current)
            value |= SDL_HAT_LEFT;
        if (it->second->hats[hatIndex * 4 + 3].current)
            value |= SDL_HAT_RIGHT;
        return value;
    }

    SDL_Joystick *Input::GetJoystickHandle(SDL_JoystickID instanceId) const
    {
        auto it = m_JoystickStates.find(instanceId);
        return (it != m_JoystickStates.end()) ? it->second->handle : nullptr;
    }

    std::vector<SDL_JoystickID> Input::GetConnectedJoystickIDs() const
    {
        std::vector<SDL_JoystickID> ids;
        ids.reserve(m_JoystickStates.size());
        for (const auto &[id, state] : m_JoystickStates)
        {
            ids.push_back(id);
        }
        return ids;
    }

    std::vector<SDL_TouchID> Input::GetConnectedTouchDeviceIDs() const
    {
        std::vector<SDL_TouchID> ids;
        ids.reserve(m_TouchStates.size());
        for (const auto &[id, state] : m_TouchStates)
        {
            ids.push_back(id);
        }
        return ids;
    }

    SDL_TouchDeviceType Input::GetTouchDeviceType(SDL_TouchID touchId) const
    {
        auto it = m_TouchStates.find(touchId);
        if (it != m_TouchStates.end())
        {
            return it->second->type;
        }
        return SDL_TOUCH_DEVICE_INVALID;
    }

    const char *Input::GetTouchDeviceName(SDL_TouchID touchId) const
    {
        auto it = m_TouchStates.find(touchId);
        if (it != m_TouchStates.end())
        {
            return it->second->name.c_str();
        }
        return nullptr;
    }

    int Input::GetNumActiveFingers(SDL_TouchID touchId) const
    {
        auto it = m_TouchStates.find(touchId);
        if (it != m_TouchStates.end())
        {
            return static_cast<int>(it->second->activeFingers.size());
        }
        return 0;
    }

    std::vector<SDL_FingerID> Input::GetActiveFingerIDs(SDL_TouchID touchId) const
    {
        std::vector<SDL_FingerID> fingerIds;
        auto it = m_TouchStates.find(touchId);
        if (it != m_TouchStates.end())
        {
            fingerIds.reserve(it->second->activeFingers.size());
            for (const auto &[fingerId, fingerState] : it->second->activeFingers)
            {
                fingerIds.push_back(fingerId);
            }
        }
        return fingerIds;
    }

    bool Input::GetFingerState(SDL_TouchID touchId, SDL_FingerID fingerId, glm::vec2 &outPos, float &outPressure) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                outPos = fingerIt->second.currentPos;
                outPressure = fingerIt->second.currentPressure;
                return true;
            }
        }
        return false;
    }

    glm::vec2 Input::GetFingerPosition(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                return fingerIt->second.currentPos;
            }
        }
        return {-1.0f, -1.0f};
    }

    float Input::GetFingerPressure(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                return fingerIt->second.currentPressure;
            }
        }
        return 0.0f;
    }

    glm::vec2 Input::GetFingerDelta(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                return glm::vec2{
                    fingerIt->second.currentPos.x - fingerIt->second.previousPos.x,
                    fingerIt->second.currentPos.y - fingerIt->second.previousPos.y};
            }
        }
        return glm::vec2{0.0f, 0.0f};
    }

    bool Input::IsFingerDown(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                return fingerIt->second.isDown;
            }
        }
        return false;
    }

    bool Input::IsFingerPressed(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                return fingerIt->second.pressedThisFrame;
            }
        }
        return false;
    }

    bool Input::IsFingerReleased(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        auto touchIt = m_TouchStates.find(touchId);
        if (touchIt != m_TouchStates.end())
        {
            auto fingerIt = touchIt->second->activeFingers.find(fingerId);
            if (fingerIt != touchIt->second->activeFingers.end())
            {
                return fingerIt->second.releasedThisFrame;
            }
        }
        return false;
    }

    bool Input::SetRelativeMouseMode(bool enabled)
    {
        if (!m_Window)
        {
            LOG_ERROR("Cannot set relative mouse mode, window handle is null.");
            return false;
        }

        // Use the new window-specific function
        if (!SDL_SetWindowRelativeMouseMode(m_Window, enabled ? true : false))
        {
            LOG_ERROR("Failed to set relative mouse mode: {}", SDL_GetError());
            return false;
        }

        m_RelativeMouseMode = enabled;
        return true;
    }

    bool Input::IsRelativeMouseMode() const
    {
        if (!m_Window)
        {
            return false;
        }
        // Query using the new window-specific function
        return SDL_GetWindowRelativeMouseMode(m_Window);
    }

    Uint32 Input::GetRelativeMouseState(float *x, float *y)
    {
        if (!Input::Get().IsRelativeMouseMode())
        {
            if (x)
            {
                *x = 0.0f;
            }
            if (y)
            {
                *y = 0.0f;
            }
            return 0;
        }

        float deltaX = 0.0f;
        float deltaY = 0.0f; 

        Uint32 buttonState = SDL_GetRelativeMouseState(&deltaX, &deltaY);

        if (x)
        {
            *x = deltaX;
        }
        if (y)
        {
            *y = deltaY;
        }

        return buttonState;
    }

}