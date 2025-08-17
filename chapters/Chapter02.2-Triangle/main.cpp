#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Chapter02_2_Application : public Base::Application
{
public:
     Chapter02_2_Application() : Application("Chapter 2.2: The Triangle (Attribute Pointers)") {}

protected:
    void setup() override
    {
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile("shaders/chapter02_2.vert", "shaders/chapter02_2.frag");

        // 1. THE DATA: We now have 3 vertices to form a triangle.
        float vertices[] = {
             0.5f, -0.5f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
             0.0f,  0.5f, 0.0f  // top
        };

        // 2. VBO and VAO setup is the same as before.
        glGenBuffers(1, &m_VboID);
        glGenVertexArrays(1, &m_VaoID);
        
        glBindVertexArray(m_VaoID);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 3. THE DECODER RING (glVertexAttribPointer) EXPLAINED:
        // This function tells the GPU how to read the raw data in the VBO.
        // Let's break down each parameter.
        glVertexAttribPointer(
            0,                  // location:  Corresponds to 'layout (location = 0)' in the shader.
            3,                  // size:      Each vertex position has 3 components (x, y, z).
            GL_FLOAT,           // type:      The data is made of 32-bit floats.
            GL_FALSE,           // normalized: Don't normalize the data.
            3 * sizeof(float),  // stride:    The size of one complete vertex. The next vertex
                                //            starts after this many bytes.
            (void*)0            // offset:    Where this attribute begins within the vertex.
                                //            Our position data starts at the very beginning.
        );
        // Finally, we must enable this attribute location.
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
        m_Shader->setVec4("u_TriangleColor", glm::make_vec4(m_TriangleColor));

        glBindVertexArray(m_VaoID);
        // Draw 3 vertices, interpreted as a single triangle.
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Triangle Settings");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Triangle Color", m_TriangleColor);
        ImGui::End();
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_TriangleColor[4] = {1.0f, 0.5f, 0.2f, 1.0f};
};

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter02_2_Application>();
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