#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Chapter02_1_Application : public Base::Application
{
public:
    Chapter02_1_Application() : Application("Chapter 2.1: A Single Point (VBO & VAO)") {}

protected:
    void setup() override
    {
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile("shaders/chapter02_1.vert", "shaders/chapter02_1.frag");

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // It's good practice to unbind the VBO and VAO when you're done configuring them,
        // to prevent accidental changes.
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->use();

        // We need this to make points larger than 1 pixel.
        // TODO IOS ERR:Use of undeclared identifier 'GL_PROGRAM_POINT_SIZE'
        glEnable(GL_PROGRAM_POINT_SIZE);

        // To draw, we just bind our "preset" (the VAO)...
        glBindVertexArray(m_VaoID);
        // ...and tell OpenGL to draw 1 vertex from our buffer as a point.
        glDrawArrays(GL_POINTS, 0, 1);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Chapter 02 Triangle & Uniforms");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::End();
    }

    void handleInput(float deltaTime) override
    {
        // Handle input events here if needed.
        // For this example, we don't have any specific input handling.
    }
    void update(float deltaTime) override
    {
        // Update logic here if needed.
        // For this example, we don't have any specific update logic.
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    GLint m_PosAttribLocation = -1;
    float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float m_TriangleColor[4] = {1.0f, 0.5f, 0.2f, 1.0f};
};

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter02_1_Application>();
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
