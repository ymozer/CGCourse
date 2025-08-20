#include "Chapter06.hpp"
#include "Log.hpp"

#include <imgui.h>

#ifdef BUILD_STANDALONE
Chapter06_Application::Chapter06_Application(std::string title, int width, int height)
    : ChapterBase(title, width, height) // Calls Base::Application constructor
{
    LOG_INFO("Chapter 06 constructed in STANDALONE mode.");
}
#else
Chapter06_Application::Chapter06_Application() 
    : ChapterBase() // Calls IChapter constructor
{
    LOG_INFO("Chapter 06 constructed in BUNDLED mode.");
}
#endif

void Chapter06_Application::setup()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter06.vert", "shaders/chapter06.frag");

    // 1. THE DATA: To draw a square (2 triangles), we need 6 vertices.
    // This is the same vertex setup as Chapter 5.
    float vertices[] = {
        // First Triangle
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f,  0.5f, 0.0f,  // top left
        // Second Triangle
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    // 2. THE VBO & VAO: Create and configure them.
    glGenBuffers(1, &m_VboID);
    glGenVertexArrays(1, &m_VaoID);

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. THE LAYOUT: Tell the VAO how to interpret the position data.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 4. THE UNIFORM: Get the location of our color uniform from the shader.
    // We must 'use' the shader program first to make it active.
    m_Shader->use();
    m_SquareColorLocation = glGetUniformLocation(m_Shader->getProgramID(), "u_SquareColor");

    // Good practice to unbind everything.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter06_Application::shutdown()
{
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    m_Shader.reset();
}

void Chapter06_Application::render()
{
    // Use the member variable for the background color.
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // Activate our shader program.
    m_Shader->use();

    // 5. SEND THE DATA: Every frame, send the color from our C++ variable
    // to the uniform location on the GPU.
    if (m_SquareColorLocation != -1)
    {
        glUniform4fv(m_SquareColorLocation, 1, m_SquareColor);
    }

    // Bind our VAO (which contains the layout for our square).
    glBindVertexArray(m_VaoID);
    // Draw the 6 vertices as two triangles to form the square.
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Chapter06_Application::renderChapterUI()
{
    ImGui::Begin("Chapter 6 Controls");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    
    // Add a new color picker that controls our square's color variable.
    ImGui::ColorEdit3("Square Color", m_SquareColor);
    ImGui::End();
}

void Chapter06_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
}
void Chapter06_Application::update(float deltaTime)
{
    // Update logic here if needed.
}