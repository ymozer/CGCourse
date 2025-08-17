#include "Chapter01.hpp"
#include "Log.hpp"

Chapter01_Application::Chapter01_Application() : Application("Chapter 01: The Basics") {}

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Chapter01_Application::renderChapterUI() {
    ImGui::Begin("Chapter 01 Controls");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::End();
}