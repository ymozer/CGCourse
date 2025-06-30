#pragma once

#include "Application.hpp"
#include <imgui.h>

class Chapter01_Application : public Base::Application
{
public:
    Chapter01_Application();

protected:
    void setup() override;
    void shutdown() override;
    void update(float deltaTime) override;
    void render() override;
    void renderChapterUI() override;

private:
    float m_ClearColor[4] = { 0.1f, 0.1f, 0.2f, 1.0f };
};