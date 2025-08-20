#include "Chapter01.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include <imgui.h>

#ifdef BUILD_STANDALONE
Chapter01_Application::Chapter01_Application(std::string title, int width, int height)
    : ChapterBase(title, width, height) // Calls Base::Application constructor
{
    LOG_INFO("Chapter 01 constructed in STANDALONE mode.");
}
#else
Chapter01_Application::Chapter01_Application() 
    : ChapterBase() // Calls IChapter constructor
{
    LOG_INFO("Chapter 01 constructed in BUNDLED mode.");
}
#endif

void Chapter01_Application::setup() {
    LOG_INFO("Chapter 01 setup complete.");
}

void Chapter01_Application::shutdown() {
    LOG_INFO("Chapter 01 shutdown.");
}

void Chapter01_Application::update(float deltaTime) {
}

void Chapter01_Application::handleInput(float deltaTime) {
}

void Chapter01_Application::render() {
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Chapter01_Application::renderChapterUI() {
    ImGui::Begin("Chapter 01 Controls");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::End();
}