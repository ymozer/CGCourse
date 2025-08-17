#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Texture.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Chapter03_Application : public Base::Application
{
public:
    Chapter03_Application() : Application("Chapter 03: Textured Square (Optimized)") {}

protected:
    void setup() override
    {
        LOG_INFO("Chapter 03 setup...");

        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile(
            "shaders/chapter03.vert",
            "shaders/chapter03.frag");

        // === 1. REDUCE VERTEX DATA ===
        // We now only define the 4 unique vertices for the square's corners.
        // This is more memory efficient.
        float vertices[] = {
            // positions         // texture coords
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right      (index 0)
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right   (index 1)
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left    (index 2)
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left       (index 3)
        };

        // === 2. CREATE AN INDEX ARRAY ===
        // This array tells OpenGL how to connect the vertices above to form triangles.
        // We are "reusing" vertices 1 and 3.
        unsigned int indices[] = {
            0, 1, 3,  // First Triangle: uses vertices at index 0, 1, and 3
            1, 2, 3   // Second Triangle: uses vertices at index 1, 2, and 3
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        
        const void* texCoordOffset = (void*)(3 * sizeof(float));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, texCoordOffset);
        glEnableVertexAttribArray(1);
        
        // IMPORTANT: The EBO binding is "remembered" by the VAO.
        // You can unbind the VBO (GL_ARRAY_BUFFER), but you must NOT unbind the EBO
        // while the VAO is still bound, or it will be disconnected from the VAO.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0); // Unbinding the VAO is safe now.

        // Load the texture
        m_Texture = std::make_unique<Base::Texture>();
        if (!m_Texture->loadFromFile("assets/images/uv.png")) {
            LOG_ERROR("Failed to load square texture!");
        }

        LOG_INFO("Chapter 03 setup complete.");
    }

    void shutdown() override
    {
        LOG_INFO("Chapter 03 shutdown.");
        glDeleteVertexArrays(1, &m_VaoID);
        glDeleteBuffers(1, &m_VboID);
        glDeleteBuffers(1, &m_EboID); // <<< NEW: Delete the EBO
        m_Shader.reset();
        m_Texture.reset();
    }

    void update(float deltaTime) override {}

    void handleInput(float deltaTime) override {}

    void render() override
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
            GL_TRIANGLES,      // mode
            6,                 // count: We are drawing 6 indices
            GL_UNSIGNED_INT,   // type: The indices are unsigned integers
            0                  // offset: Start at the beginning of the index buffer
        );
        
        glBindVertexArray(0);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Optimized Textured Square");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Square Tint Color", m_SquareTintColor);
        ImGui::End();
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;

    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    GLuint m_EboID = 0; // <<< NEW: Member variable for the Element Buffer Object
    
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_SquareTintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter03_Application>();
        app->run();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("A fatal error occurred: {}", e.what());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Application Error",
            e.what(),
            NULL);
        return -1;
    }
    return 0;
}