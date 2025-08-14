#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Base
{
    class ParallelEventBus;
    struct GamepadState;
    struct JoystickState;
    struct ButtonState;
    struct AxisState;
    struct FingerState;
    struct TouchDeviceState;
    class Input
    {
    public:
        static Input &Get();

        Input() = default;
        ~Input() = default;

        bool Initialize(SDL_Window* window, ParallelEventBus &bus, bool imguiEnabled);
        void Shutdown();
        void PrepareForFrame();
        void ProcessEvent(const SDL_Event &event);
        void Update();

        bool IsKeyDown(SDL_Scancode scancode) const;
        bool IsKeyPressed(SDL_Scancode scancode) const;
        bool IsKeyReleased(SDL_Scancode scancode) const;

        bool IsMouseButtonDown(int button) const;
        bool IsMouseButtonPressed(int button) const;
        bool IsMouseButtonReleased(int button) const;

        glm::vec2 GetMousePosition() const;
        glm::vec2 GetMouseDelta() const;
        glm::vec2 GetMouseWheel() const;
        bool IsRelativeMouseMode() const;
        bool SetRelativeMouseMode(bool enabled);

        bool IsGamepadConnected(SDL_JoystickID instanceId) const;
        bool IsGamepadButtonDown(SDL_JoystickID instanceId, SDL_GamepadButton button) const;
        bool IsGamepadButtonPressed(SDL_JoystickID instanceId, SDL_GamepadButton button) const;
        bool IsGamepadButtonReleased(SDL_JoystickID instanceId, SDL_GamepadButton button) const;
        float GetGamepadAxis(SDL_JoystickID instanceId, SDL_GamepadAxis axis) const;
        SDL_Gamepad *GetGamepadHandle(SDL_JoystickID instanceId) const;
        std::vector<SDL_JoystickID> GetConnectedGamepadIDs() const;

        bool IsJoystickConnected(SDL_JoystickID instanceId) const;
        bool IsJoystickButtonDown(SDL_JoystickID instanceId, int buttonIndex) const;
        bool IsJoystickButtonPressed(SDL_JoystickID instanceId, int buttonIndex) const;
        bool IsJoystickButtonReleased(SDL_JoystickID instanceId, int buttonIndex) const;
        float GetJoystickAxis(SDL_JoystickID instanceId, int axisIndex) const;
        Uint8 GetJoystickHat(SDL_JoystickID instanceId, int hatIndex) const;
        SDL_Joystick *GetJoystickHandle(SDL_JoystickID instanceId) const;
        std::vector<SDL_JoystickID> GetConnectedJoystickIDs() const;

        std::vector<SDL_TouchID> GetConnectedTouchDeviceIDs() const;
        SDL_TouchDeviceType GetTouchDeviceType(SDL_TouchID touchId) const;
        const char *GetTouchDeviceName(SDL_TouchID touchId) const;
        int GetNumActiveFingers(SDL_TouchID touchId) const;
        std::vector<SDL_FingerID> GetActiveFingerIDs(SDL_TouchID touchId) const;

        bool GetFingerState(SDL_TouchID touchId, SDL_FingerID fingerId,
                            glm::vec2 &outPos, float &outPressure) const;
        glm::vec2 GetFingerPosition(SDL_TouchID touchId, SDL_FingerID fingerId) const;
        float GetFingerPressure(SDL_TouchID touchId, SDL_FingerID fingerId) const;
        glm::vec2 GetFingerDelta(SDL_TouchID touchId, SDL_FingerID fingerId) const;

        bool IsFingerDown(SDL_TouchID touchId, SDL_FingerID fingerId) const;
        bool IsFingerPressed(SDL_TouchID touchId, SDL_FingerID fingerId) const;
        bool IsFingerReleased(SDL_TouchID touchId, SDL_FingerID fingerId) const;

        void SetGamepadAxisDeadzone(float deadzone) { m_GamepadAxisDeadzone = deadzone; }
        void SetJoystickAxisDeadzone(float deadzone) { m_JoystickAxisDeadzone = deadzone; }


    private:
        SDL_Window *m_Window = nullptr;
        ParallelEventBus *m_EventBus = nullptr;
        static std::unique_ptr<Input> s_InputInstance;

        Input(const Input &) = delete;
        Input &operator=(const Input &) = delete;
        Input(Input &&) = delete;
        Input &operator=(Input &&) = delete;

        void AddDevice(SDL_JoystickID which);
        void RemoveDevice(SDL_JoystickID which);
        float ApplyDeadzone(Sint16 value, float deadzoneThreshold) const;

        bool m_imguiEnabled = false;
        bool m_RelativeMouseMode = false;
        float m_GamepadAxisDeadzone = 0.15f;
        float m_JoystickAxisDeadzone = 0.15f;
        int m_NumKeys = 0;
        const bool *m_CurrentKeyState = nullptr;
        std::vector<Uint8> m_PreviousKeyState;

        Uint32 m_CurrentMouseButtonState = 0;
        Uint32 m_PreviousMouseButtonState = 0;
        glm::vec2 m_CurrentMousePos = {0, 0};
        glm::vec2 m_PreviousMousePos = {0, 0};
        glm::vec2 m_MouseDelta = {0, 0};
        glm::vec2 m_MouseWheelDelta = {0.0f, 0.0f};
        std::unordered_map<SDL_JoystickID, std::unique_ptr<GamepadState>> m_GamepadStates;
        std::unordered_map<SDL_JoystickID, std::unique_ptr<JoystickState>> m_JoystickStates;
        std::unordered_map<SDL_TouchID, std::unique_ptr<TouchDeviceState>> m_TouchStates;
    };

}