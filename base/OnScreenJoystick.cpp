#include "OnScreenJoystick.hpp"
#include "Input.hpp"
#include <imgui.h>

namespace Base
{
    // Implement the new, more flexible constructor
    OnScreenJoystick::OnScreenJoystick(Alignment alignment, const ImVec2& positionOffset, float outerRadius, float innerRadius)
        : m_Alignment(alignment),
          m_Position(positionOffset),
          m_OuterRadius(outerRadius),
          m_InnerRadius(innerRadius),
          m_IsActive(false),
          m_TouchId(0),
          m_FingerId(0),
          m_Direction(0.0f, 0.0f),
          m_BaseColor(0.5f, 0.5f, 0.5f, 0.3f),
          m_KnobColor(0.8f, 0.8f, 0.8f, 0.5f)
    {
    }

    void OnScreenJoystick::update(const ImVec2 &viewportPos, const ImVec2 &viewportSize)
    {
        auto &input = Input::Get();
        const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

        if (m_Alignment == Alignment::BottomLeft)
        {
            m_CenterPos = ImVec2(viewportPos.x + m_Position.x,
                                 viewportPos.y + viewportSize.y + m_Position.y);
        }
        else
        {
            m_CenterPos = ImVec2(viewportPos.x + viewportSize.x - m_Position.x,
                                 viewportPos.y + viewportSize.y + m_Position.y);
        }

        m_KnobPos = m_CenterPos;

        if (m_IsActive)
        {
            if (input.IsFingerReleased(m_TouchId, m_FingerId))
            {
                m_IsActive = false;
                m_Direction = {0.0f, 0.0f};
                return;
            }
            glm::vec2 fingerPosGLM = input.GetFingerPosition(m_TouchId, m_FingerId);
            ImVec2 fingerPos = ImVec2(fingerPosGLM.x * windowSize.x, fingerPosGLM.y * windowSize.y);
            ImVec2 delta = {fingerPos.x - m_CenterPos.x, fingerPos.y - m_CenterPos.y};
            float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);
            if (distance > m_OuterRadius)
            {
                m_KnobPos.x = m_CenterPos.x + (delta.x / distance) * m_OuterRadius;
                m_KnobPos.y = m_CenterPos.y + (delta.y / distance) * m_OuterRadius;
            }
            else
            {
                m_KnobPos = fingerPos;
            }
            ImVec2 directionVec = {m_KnobPos.x - m_CenterPos.x, m_KnobPos.y - m_CenterPos.y};
            m_Direction.x = directionVec.x / m_OuterRadius;
            m_Direction.y = directionVec.y / m_OuterRadius;
        }
        else
        {
            auto touchDeviceIds = input.GetConnectedTouchDeviceIDs();
            for (const auto &touchId : touchDeviceIds)
            {
                auto fingerIds = input.GetActiveFingerIDs(touchId);
                for (const auto &fingerId : fingerIds)
                {
                    if (input.IsFingerPressed(touchId, fingerId))
                    {
                        glm::vec2 fingerPosGLM = input.GetFingerPosition(touchId, fingerId);
                        ImVec2 fingerPos = ImVec2(fingerPosGLM.x * windowSize.x, fingerPosGLM.y * windowSize.y);
                        float distance = sqrtf(powf(fingerPos.x - m_CenterPos.x, 2) + powf(fingerPos.y - m_CenterPos.y, 2));
                        if (distance <= m_OuterRadius)
                        {
                            m_IsActive = true;
                            m_TouchId = touchId;
                            m_FingerId = fingerId;
                            return;
                        }
                    }
                }
            }
        }
    }

    void OnScreenJoystick::render()
    {
        ImDrawList *draw_list = ImGui::GetForegroundDrawList();
        draw_list->AddCircleFilled(m_CenterPos, m_OuterRadius, ImGui::ColorConvertFloat4ToU32(m_BaseColor));
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