#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include <memory>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Chapter02_Application : public Base::Application
{
public:
    Chapter02_Application() : Application("Chapter 02: Hello Triangle") {}

protected:
    void setup() override
    {
        LOG_INFO("Chapter 02 setup...");

        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile(
            "shaders/chapter02.vert", 
            "shaders/chapter02.frag"
        );

        float vertices[] = {
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
            0.0f, 0.5f, 0.0f    // top
        };

        glGenVertexArrays(1, &m_VaoID);
        glGenBuffers(1, &m_VboID);

        glBindVertexArray(m_VaoID);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        LOG_INFO("Chapter 02 setup complete.");
    }

    void shutdown() override
    {
        LOG_INFO("Chapter 02 shutdown.");
        glDeleteVertexArrays(1, &m_VaoID);
        glDeleteBuffers(1, &m_VboID);
        m_Shader.reset();
    }

    void update(float deltaTime) override
    {
    }

    void render() override
    {
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Shader->use();
        m_Shader->setVec4("u_TriangleColor", glm::make_vec4(m_TriangleColor));

        glBindVertexArray(m_VaoID);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Chapter 01 Controls");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Triangle Color", m_TriangleColor);
        ImGui::End();
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    float m_ClearColor[3] = {0.1f, 0.1f, 0.2f};
    float m_TriangleColor[4] = {1.0f, 0.5f, 0.2f, 1.0f}; // Orange, with alpha
};

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

   try
    {
        // Now it's safe to create the application.
        auto app = std::make_unique<Chapter02_Application>();
        app->run();
    }
    catch (const std::exception &e)
    {
        // It is now 100% safe to log this error.
        LOG_ERROR("A fatal error occurred: {}", e.what());
        
        // On mobile, the console isn't visible, so show a popup.
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Application Error",
            e.what(),
            NULL
        );
        return -1;
    }
    return 0;
}