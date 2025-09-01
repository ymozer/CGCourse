#pragma once

#include "imgui.h"
#include "glm/vec2.hpp"
#include "SDL3/SDL.h"

namespace Base {
class OnScreenJoystick
{
public:
    void update(const ImVec2& viewportPos, const ImVec2& viewportSize);
    void render();
    glm::vec2 getDirection() const { return m_IsActive ? m_Direction : glm::vec2(0.0f); }

private:
    // Configuration
    const float m_OuterRadius = 100.0f; // Outer circle radius
    const float m_InnerRadius = 40.0f;  // Knob radius
    const ImVec2 m_Position = {150.0f, -150.0f}; // Position relative to bottom-left of viewport
    const ImVec4 m_BaseColor = {1.0f, 1.0f, 1.0f, 0.2f};
    const ImVec4 m_KnobColor = {1.0f, 1.0f, 1.0f, 0.4f};

    // State
    bool m_IsActive = false;
    SDL_TouchID m_TouchId = 0;
    SDL_FingerID m_FingerId = 0;
    ImVec2 m_CenterPos = {0,0};
    ImVec2 m_KnobPos = {0,0};
    glm::vec2 m_Direction = {0.0f, 0.0f};
};

} // namespace Base
