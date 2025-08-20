#include "Chapter02.hpp"
#include "Log.hpp"
#include <imgui.h>


#ifdef BUILD_STANDALONE
Chapter02_Application::Chapter02_Application(std::string title, int width, int height)
    : ChapterBase(title, width, height) // Calls Base::Application constructor
{
    LOG_INFO("Chapter 02 constructed in STANDALONE mode.");
}
#else
Chapter02_Application::Chapter02_Application() 
    : ChapterBase() // Calls IChapter constructor
{
    LOG_INFO("Chapter 02 constructed in BUNDLED mode.");
}
#endif

void Chapter02_Application::setup()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter02.vert", "shaders/chapter02.frag");

    // 1. THE DATA: Define the data for a single point at the center of the screen.
    float vertices[] = {
        0.0f, 0.0f, 0.0f // A single vertex with X, Y, Z coordinates
    };

    // 2. THE VBO (Vertex Buffer Object): A memory buffer on the GPU.
    // We create a buffer ID and then upload our 'vertices' data into it.
    glGenBuffers(1, &m_VboID);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. THE VAO (Vertex Array Object): A "preset" that stores our data layout.
    // In modern OpenGL, we MUST use a VAO. We create and bind one here.
    // All subsequent calls to glVertexAttribPointer and glEnableVertexAttribArray
    // will be "recorded" into this VAO.
    glGenVertexArrays(1, &m_VaoID);
    glBindVertexArray(m_VaoID);

    // 4. THE (TEMPORARILY) "MAGIC" LINK:
    // These two lines connect the data in our VBO to our shader.
    // Don't worry about the parameters yet. We will dedicate the entire
    // next chapter to explaining them. For now, just know that this sets up
    // the connection to 'layout (location = 0)' in our vertex shader.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // It's good practice to unbind the VBO and VAO when you're done configuring them,
    // to prevent accidental changes.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter02_Application::shutdown()
{
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    m_Shader.reset();
}

void Chapter02_Application::render()
{
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Shader->use();

#if PLATFORM_DESKTOP
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif

    // To draw, we just bind our "preset" (the VAO)...
    glBindVertexArray(m_VaoID);
    // ...and tell OpenGL to draw 1 vertex from our buffer as a point.
    glDrawArrays(GL_POINTS, 0, 1);
}

void Chapter02_Application::renderChapterUI()
{
    ImGui::Begin("Chapter 02: Point");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::End();
}

void Chapter02_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
    // For this example, we don't have any specific input handling.
}
void Chapter02_Application::update(float deltaTime)
{
    // Update logic here if needed.
    // For this example, we don't have any specific update logic.
}