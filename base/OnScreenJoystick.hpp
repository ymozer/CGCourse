#pragma once
#include <imgui.h>
#include <glm/glm.hpp>

namespace Base
{
    class OnScreenJoystick
    {
    public:
        enum class Alignment
        {
            BottomLeft,
            BottomRight
        };

        OnScreenJoystick(Alignment alignment, const ImVec2& positionOffset, float outerRadius, float innerRadius);

        void update(const ImVec2& viewportPos, const ImVec2& viewportSize);
        void render();
        glm::vec2 getDirection() const { return m_Direction; }
        long long getFingerId() const { return m_FingerId; }
        bool IsActive() const { return m_IsActive; }


    private:
        Alignment m_Alignment; // Store the alignment
        ImVec2 m_Position;     // This is now an *offset* from the aligned corner
        float m_OuterRadius;
        float m_InnerRadius;
        ImVec4 m_BaseColor;
        ImVec4 m_KnobColor;

        // State
        bool m_IsActive;
        int m_TouchId;
        long long m_FingerId;
        glm::vec2 m_Direction;

        // Calculated positions
        ImVec2 m_CenterPos;
        ImVec2 m_KnobPos;
    };
}