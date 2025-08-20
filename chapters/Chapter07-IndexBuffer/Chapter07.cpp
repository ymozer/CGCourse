#include "Chapter07.hpp"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Chapter07_Application::Chapter07_Application() : IChapter() {}

void Chapter07_Application::setup()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter07.vert", "shaders/chapter07.frag");

    // 1. THE DATA: We now only specify the 4 UNIQUE vertices that make up the square.
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right (index 0)
        0.5f, -0.5f, 0.0f,  // bottom right (index 1)
        -0.5f, -0.5f, 0.0f, // bottom left (index 2)
        -0.5f, 0.5f, 0.0f   // top left (index 3)
    };

    // 2. THE RECIPE (Indices): This array tells OpenGL the order to draw the vertices
    // from the VBO to form triangles. We are reusing vertices!
    unsigned int indices[] = {
        0, 1, 3, // First Triangle: top-right, bottom-right, top-left
        1, 2, 3  // Second Triangle: bottom-right, bottom-left, top-left
    };

    glGenVertexArrays(1, &m_VaoID);
    glGenBuffers(1, &m_VboID);
    glGenBuffers(1, &m_EboID); // Create a buffer for the indices

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind the Element Buffer Object and upload the index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // The vertex attribute setup is the same. It describes the VBO, not the EBO.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // NOTE: The EBO is bound while the VAO is bound. This "links" the EBO to the VAO.
    // So, when we later bind this VAO, the EBO is automatically bound as well.
    // We can unbind the GL_ARRAY_BUFFER, but NOT the GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter07_Application::shutdown()
{
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    glDeleteBuffers(1, &m_EboID);
    m_Shader.reset();
}

void Chapter07_Application::render()
{
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Shader->use();
    m_Shader->setVec4("u_SquareColor", glm::make_vec4(m_SquareColor));

    glBindVertexArray(m_VaoID);

    // 3. THE NEW DRAW CALL: glDrawElements
    // This tells OpenGL to draw using the currently bound EBO.
    glDrawElements(
        GL_TRIANGLES,    // mode: We're still drawing triangles.
        6,               // count: We're drawing 6 indices in total.
        GL_UNSIGNED_INT, // type: The indices are of type unsigned int.
        0                // offset: Start from the beginning of the index buffer.
    );
}

void Chapter07_Application::renderChapterUI()
{
    ImGui::Begin("Scene Controls");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::ColorEdit3("Square Color", m_SquareColor);
    ImGui::End();
}

void Chapter07_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
}
void Chapter07_Application::update(float deltaTime)
{
    // Update logic here if needed.
}