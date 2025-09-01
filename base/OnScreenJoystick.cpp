#include "OnScreenJoystick.hpp"
#include "Input.hpp"


namespace Base
{
    void OnScreenJoystick::update(const ImVec2 &viewportPos, const ImVec2 &viewportSize)
    {
        auto &input = Input::Get();

        // Calculate the absolute center position of the joystick on the screen
        m_CenterPos = ImVec2(viewportPos.x + m_Position.x,
                             viewportPos.y + viewportSize.y + m_Position.y);
        m_KnobPos = m_CenterPos;

        if (m_IsActive)
        {
            // If joystick is active, check if the controlling finger was released
            if (input.IsFingerReleased(m_TouchId, m_FingerId))
            {
                m_IsActive = false;
                m_Direction = {0.0f, 0.0f};
                return;
            }

            // Get current finger position
            glm::vec2 fingerPosGLM = input.GetFingerPosition(m_TouchId, m_FingerId);
            ImVec2 fingerPos = ImVec2(fingerPosGLM.x * viewportSize.x + viewportPos.x,
                                      fingerPosGLM.y * viewportSize.y + viewportPos.y);

            ImVec2 delta = {fingerPos.x - m_CenterPos.x, fingerPos.y - m_CenterPos.y};
            float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);

            if (distance > m_OuterRadius)
            {
                // Clamp knob position to the edge of the base circle
                m_KnobPos.x = m_CenterPos.x + (delta.x / distance) * m_OuterRadius;
                m_KnobPos.y = m_CenterPos.y + (delta.y / distance) * m_OuterRadius;
            }
            else
            {
                m_KnobPos = fingerPos;
            }

            // Calculate direction vector
            ImVec2 directionVec = {m_KnobPos.x - m_CenterPos.x, m_KnobPos.y - m_CenterPos.y};
            m_Direction.x = directionVec.x / m_OuterRadius;
            m_Direction.y = directionVec.y / m_OuterRadius;
        }
        else
        {
            // If joystick is not active, check for a new touch
            auto touchDeviceIds = input.GetConnectedTouchDeviceIDs();
            for (const auto &touchId : touchDeviceIds)
            {
                auto fingerIds = input.GetActiveFingerIDs(touchId);
                for (const auto &fingerId : fingerIds)
                {
                    if (input.IsFingerPressed(touchId, fingerId))
                    {
                        glm::vec2 fingerPosGLM = input.GetFingerPosition(touchId, fingerId);
                        ImVec2 fingerPos = ImVec2(fingerPosGLM.x * viewportSize.x + viewportPos.x,
                                                  fingerPosGLM.y * viewportSize.y + viewportPos.y);

                        float distance = sqrtf(powf(fingerPos.x - m_CenterPos.x, 2) + powf(fingerPos.y - m_CenterPos.y, 2));

                        // Activate if touch is within the joystick's radius
                        if (distance <= m_OuterRadius)
                        {
                            m_IsActive = true;
                            m_TouchId = touchId;
                            m_FingerId = fingerId;
                            return; // Found a finger, no need to check others
                        }
                    }
                }
            }
        }
    }

    void OnScreenJoystick::render()
    {
        // Use the background draw list to render behind ImGui windows
        ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

        // Draw the outer circle (base)
        draw_list->AddCircleFilled(m_CenterPos, m_OuterRadius, ImGui::ColorConvertFloat4ToU32(m_BaseColor));

        // Draw the inner circle (knob)
        if (m_IsActive)
        {
            draw_list->AddCircleFilled(m_KnobPos, m_InnerRadius, ImGui::ColorConvertFloat4ToU32(m_KnobColor));
        }
        else
        {
            draw_list->AddCircleFilled(m_CenterPos, m_InnerRadius, ImGui::ColorConvertFloat4ToU32(m_KnobColor));
        }
    }
}