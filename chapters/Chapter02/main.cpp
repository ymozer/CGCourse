#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"

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
            "shaders/chapter02.frag");

        m_Shader->use();
        m_PosAttribLocation = glGetAttribLocation(m_Shader->getProgramID(), "aPos");
        if (m_PosAttribLocation == -1)
        {
            LOG_ERROR("Could not find attribute 'aPos' in the shader!");
        }
        LOG_INFO("aPos location found: {}", m_PosAttribLocation);

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
        glVertexAttribPointer(m_PosAttribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(m_PosAttribLocation);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_Camera.setProjection(45.0f, (float)getWidth() / (float)getHeight(), 0.1f, 100.0f);

        subscribeToKeys([this](Base::KeyPressedEvent &e)
                        {
            // NOTE: The mutex is now in the Chapter class, which is correct!
            std::lock_guard<std::mutex> lock(m_CameraInputMutex);
            switch(e.keyCode) {
                case SDLK_W: m_CameraInput.moveForward = 1.0f; break;
                case SDLK_S: m_CameraInput.moveForward = -1.0f; break;
                case SDLK_A: m_CameraInput.moveRight = -1.0f; break;
                case SDLK_D: m_CameraInput.moveRight = 1.0f; break;
                case SDLK_SPACE: m_CameraInput.moveUp = 1.0f; break;
                case SDLK_LCTRL: m_CameraInput.moveUp = -1.0f; break;
            } });
        // Don't forget to subscribe to KeyReleasedEvent too!

        // --- Subscribe to Mouse Move Events ---
        subscribeToMouse([this](Base::MouseMovedEvent &e)
                         {
                             std::lock_guard<std::mutex> lock(m_CameraInputMutex);
                             m_CameraInput.mouseDeltaX = e.xrel;
                             m_CameraInput.mouseDeltaY = -e.yrel; // Invert Y-axis
                         });

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
        // --- Update Step ---  
        // 1. Clear the screen.
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. Update the camera's view and projection matrices.
        m_Shader->use();
        m_Shader->setMat4("u_View", m_Camera.getViewMatrix());
        

        // TODO: Frustum culling

        // --- Camera Update Step ---
        // 1. Atomically get the current input state and reset the shared state for the next frame.
        CameraInput currentFrameInput;
        {
            std::lock_guard<std::mutex> lock(m_CameraInputMutex);
            currentFrameInput = m_CameraInput;
            // Reset mouse delta, as it's an event, not a state.
            // Keyboard state should be reset by KeyReleased events.
            m_CameraInput.mouseDeltaX = 0.0f;
            m_CameraInput.mouseDeltaY = 0.0f;
        }

        // 2. Update the camera on the main thread with the captured input.
        m_Camera.update(currentFrameInput, deltaTime);


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
        ImGui::Begin("Chapter 02 Triangle & Uniforms");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Triangle Color", m_TriangleColor);
        ImGui::End();
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    Camera m_Camera;
    CameraInput m_CameraInput;
    std::mutex m_CameraInputMutex;
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
        auto app = std::make_unique<Chapter02_Application>();
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