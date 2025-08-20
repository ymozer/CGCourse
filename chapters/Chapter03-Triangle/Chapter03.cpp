#include "Chapter03.hpp"
#include <imgui.h>

Chapter03_Application::Chapter03_Application() : IChapter() {}

void Chapter03_Application::setup()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter03.vert", "shaders/chapter03.frag");

    // 1. THE DATA: We now have 3 vertices to form a triangle.
    float vertices[] = {
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
         0.0f, 0.5f, 0.0f    // top
    };

    // 2. VBO and VAO setup is the same as before.
    glGenBuffers(1, &m_VboID);
    glGenVertexArrays(1, &m_VaoID);

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. THE DECODER RING (glVertexAttribPointer) EXPLAINED:
    // This function tells the GPU how to read the raw data in the VBO.
    // Let's break down each parameter.
    glVertexAttribPointer(
        0,                 // location:  Corresponds to 'layout (location = 0)' in the shader.
        3,                 // size:      Each vertex position has 3 components (x, y, z).
        GL_FLOAT,          // type:      The data is made of 32-bit floats.
        GL_FALSE,          // normalized: Don't normalize the data.
        3 * sizeof(float), // stride:    The size of one complete vertex. The next vertex
                           //            starts after this many bytes.
        (void *)0          // offset:    Where this attribute begins within the vertex.
                           //            Our position data starts at the very beginning.
    );
    // Finally, we must enable this attribute location.
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter03_Application::shutdown()
{
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    m_Shader.reset();
}

void Chapter03_Application::render()
{
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(m_VaoID);
    m_Shader->use();
    // Draw 3 vertices, interpreted as a single triangle.
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Chapter03_Application::renderChapterUI()
{
    ImGui::Begin("Scene Settings");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::End();
}

void Chapter03_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
    // For this example, we don't have any specific input handling.
}
void Chapter03_Application::update(float deltaTime)
{
    // Update logic here if needed.
    // For this example, we don't have any specific update logic.
}