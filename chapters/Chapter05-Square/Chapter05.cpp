#include "Chapter05.hpp"
#include <imgui.h>

Chapter05_Application::Chapter05_Application() : IChapter() {}

void Chapter05_Application::setup()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter05.vert", "shaders/chapter05.frag");

    // 1. THE DATA: To draw a square (2 triangles), we need 6 vertices.
    // Notice the duplication! The top-right and bottom-left vertices
    // are listed twice. For complex models, this is a huge waste of memory.
    float vertices[] = {
        // First Triangle
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, 0.5f, 0.0f,  // top left
        // Second Triangle
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    glGenBuffers(1, &m_VboID);
    glGenVertexArrays(1, &m_VaoID);

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // The attribute setup is simple again, just like the first triangle.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter05_Application::shutdown()
{
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    m_Shader.reset();
}

void Chapter05_Application::render()
{
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Shader->use();

    glBindVertexArray(m_VaoID);
    // We now have to draw 6 vertices to create our square.
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Chapter05_Application::renderChapterUI()
{
    ImGui::Begin("Square Settings");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::End();
}

void Chapter05_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
    // For this example, we don't have any specific input handling.
}
void Chapter05_Application::update(float deltaTime)
{
    // Update logic here if needed.
    // For this example, we don't have any specific update logic.
}
