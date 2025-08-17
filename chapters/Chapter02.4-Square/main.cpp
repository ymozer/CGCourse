#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Chapter02_4_Application : public Base::Application
{
public:
    Chapter02_4_Application() : Application("Chapter 2.4: A Square (The Problem)") {}

protected:
    void setup() override
    {
        // We can reuse the simple shaders from Chapter 2.2 for this example.
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile("shaders/chapter02_2.vert", "shaders/chapter02_2.frag");

        // 1. THE DATA: To draw a square (2 triangles), we need 6 vertices.
        // Notice the duplication! The top-right and bottom-left vertices
        // are listed twice. For complex models, this is a huge waste of memory.
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

        glGenBuffers(1, &m_VboID);
        glGenVertexArrays(1, &m_VaoID);
        
        glBindVertexArray(m_VaoID);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // The attribute setup is simple again, just like the first triangle.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &m_VaoID);
        glDeleteBuffers(1, &m_VboID);
        m_Shader.reset();
    }

    void render() override
    {
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->use();
        m_Shader->setVec4("u_TriangleColor", glm::make_vec4(m_SquareColor));

        glBindVertexArray(m_VaoID);
        // We now have to draw 6 vertices to create our square.
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Square Settings");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Square Color", m_SquareColor);
        ImGui::End();
    }

    void handleInput(float deltaTime) override
    {
        // Handle input events here if needed.
    }

    void update(float deltaTime) override
    {
        // Update logic here if needed.
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_SquareColor[4] = {0.2f, 0.5f, 1.0f, 1.0f};
};


int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter02_4_Application>();
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