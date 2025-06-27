#pragma once

#include "Application.hpp" // Use relative path if your includes are set up for it

// Forward declare ImGui, if needed, or include the header
#include <imgui.h>

class Chapter01_Application : public Base::Application
{
public:
    Chapter01_Application(); // Constructor declaration

protected:
    // Override declarations for all required virtual functions
    void setup() override;
    void shutdown() override;
    void update(float deltaTime) override;
    void render() override;
    void renderChapterUI() override;

private:
    float m_ClearColor[4] = { 0.1f, 0.1f, 0.2f, 1.0f };
};