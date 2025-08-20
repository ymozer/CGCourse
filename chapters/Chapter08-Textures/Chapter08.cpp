#include "Chapter08.hpp"
#include "Log.hpp"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Chapter08_Application::Chapter08_Application() : IChapter() {}

void Chapter08_Application::setup()
{
    LOG_INFO("Chapter 08 setup...");

    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile(
        "shaders/chapter08.vert",
        "shaders/chapter08.frag");

    // === 1. REDUCE VERTEX DATA ===
    // We now only define the 4 unique vertices for the square's corners.
    // This is more memory efficient.
    float vertices[] = {
        // positions         // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right      (index 0)
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right   (index 1)
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left    (index 2)
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left       (index 3)
    };

    // === 2. CREATE AN INDEX ARRAY ===
    // This array tells OpenGL how to connect the vertices above to form triangles.
    // We are "reusing" vertices 1 and 3.
    unsigned int indices[] = {
        0, 1, 3, // First Triangle: uses vertices at index 0, 1, and 3
        1, 2, 3  // Second Triangle: uses vertices at index 1, 2, and 3
    };

    glGenVertexArrays(1, &m_VaoID);
    glGenBuffers(1, &m_VboID);
    glGenBuffers(1, &m_EboID); // <<< NEW: Generate a buffer for the EBO

    glBindVertexArray(m_VaoID);

    // --- Configure VBO (for vertex data) ---
    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // --- Configure EBO (for index data) ---
    // The target for an index buffer is GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // --- Configure Vertex Attributes (this describes the VBO's layout) ---
    const GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);

    const void *texCoordOffset = (void *)(3 * sizeof(float));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, texCoordOffset);
    glEnableVertexAttribArray(1);

    // IMPORTANT: The EBO binding is "remembered" by the VAO.
    // You can unbind the VBO (GL_ARRAY_BUFFER), but you must NOT unbind the EBO
    // while the VAO is still bound, or it will be disconnected from the VAO.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbinding the VAO is safe now.

    // Load the texture
    m_Texture = std::make_unique<Base::Texture>();
    if (!m_Texture->loadFromFile("images/uv.png"))
    {
        LOG_ERROR("Failed to load square texture!");
    }

    LOG_INFO("Chapter 08 setup complete.");
}

void Chapter08_Application::shutdown()
{
    LOG_INFO("Chapter 08 shutdown.");
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    glDeleteBuffers(1, &m_EboID); // <<< NEW: Delete the EBO
    m_Shader.reset();
    m_Texture.reset();
}

void Chapter08_Application::render()
{
    // No depth test needed for a flat 2D square
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Shader->use();
    m_Shader->setInt("u_Texture", 0);
    m_Shader->setVec4("u_CubeTintColor", glm::make_vec4(m_SquareTintColor));
    m_Texture->bind(0);

    glBindVertexArray(m_VaoID);

    // === 4. CHANGE THE DRAW CALL ===
    // Instead of glDrawArrays, we use glDrawElements.
    // This tells OpenGL to use the EBO that is linked to our currently bound VAO.
    glDrawElements(
        GL_TRIANGLES,    // mode
        6,               // count: We are drawing 6 indices
        GL_UNSIGNED_INT, // type: The indices are unsigned integers
        0                // offset: Start at the beginning of the index buffer
    );

    glBindVertexArray(0);
}

void Chapter08_Application::renderChapterUI()
{
    ImGui::Begin("Scene Controls");
    ImGui::ColorEdit3("Background Color", m_ClearColor);
    ImGui::ColorEdit4("Square Tint Color", m_SquareTintColor);
    ImGui::End();
}

void Chapter08_Application::handleInput(float deltaTime)
{
    // Handle input events here if needed.
}
void Chapter08_Application::update(float deltaTime)
{
    // Update logic here if needed.
}