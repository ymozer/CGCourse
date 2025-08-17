#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


class Chapter02_3_Application : public Base::Application
{
public:
    Chapter02_3_Application() : Application("Chapter 2.3: A Colorful Triangle") {}

protected:
    void setup() override
    {
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile("shaders/chapter02_3.vert", "shaders/chapter02_3.frag");

        // 1. THE DATA: We now interleave position and color data.
        // Each complete vertex now has 6 floats.
        float vertices[] = {
            // positions         // colors
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right (Red)
            -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left  (Green)
             0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top          (Blue)
        };

        glGenBuffers(1, &m_VboID);
        glGenVertexArrays(1, &m_VaoID);
        
        glBindVertexArray(m_VaoID);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 2. THE DECODER RING (Part 1 - Position):
        // The stride is now 6 floats because each vertex is larger.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 3. THE DECODER RING (Part 2 - Color):
        // We add a SECOND attribute pointer for our color data.
        glVertexAttribPointer(
            1,                  // location: Corresponds to 'layout (location = 1)' for aColor.
            3,                  // size:     Each color has 3 components (r, g, b).
            GL_FLOAT,           // type:     The data is floats.
            GL_FALSE,           // normalized: Don't normalize.
            6 * sizeof(float),  // stride:   The total size of a vertex is still 6 floats.
            (void*)(3 * sizeof(float)) // offset: The color data starts AFTER the 3 position floats.
        );
        glEnableVertexAttribArray(1);

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
        glBindVertexArray(m_VaoID);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
};

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter02_3_Application>();
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