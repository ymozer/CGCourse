#include "Chapter04.hpp"
#include <imgui.h>

Chapter04_Application::Chapter04_Application() : IChapter() {}

void Chapter04_Application::setup()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter04.vert", "shaders/chapter04.frag");

    // 1. THE DATA: We now interleave position and color data.
    // Each complete vertex now has 6 floats.
    float vertices[] = {
         // positions         // colors
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right (Red)
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left  (Green)
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // top          (Blue)
    };

    glGenBuffers(1, &m_VboID);
    glGenVertexArrays(1, &m_VaoID);

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 2. THE DECODER RING (Part 1 - Position):
    // The stride is now 6 floats because each vertex is larger.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 3. THE DECODER RING (Part 2 - Color):
    // We add a SECOND attribute pointer for our color data.
    glVertexAttribPointer(
        1,                          // location: Corresponds to 'layout (location = 1)' for aColor.
        3,                          // size:     Each color has 3 components (r, g, b).
        GL_FLOAT,                   // type:     The data is floats.
        GL_FALSE,                   // normalized: Don't normalize.
        6 * sizeof(float),          // stride:   The total size of a vertex is still 6 floats.
        (void *)(3 * sizeof(float)) // offset: The color data starts AFTER the 3 position floats.
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter04_Application::shutdown()
{
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    m_Shader.reset();
}

void Chapter04_Application::render()
{
    // Use the member variable for the background color.
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // Activate our shader program.
    m_Shader->use();

    // Bind our VAO (which contains the layout for our triangle).
    glBindVertexArray(m_VaoID);
    // Draw the 3 vertices as a single triangle.
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Chapter04_Application::renderChapterUI()
{
    ImGui::Begin("Scene Settings");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::End();
}

void Chapter04_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
    // For this example, we don't have any specific input handling.
}
void Chapter04_Application::update(float deltaTime)
{
    // Update logic here if needed.
    // For this example, we don't have any specific update logic.
}