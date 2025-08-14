#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"
#include "Camera.hpp"
#include "Input.hpp"

#include <memory>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Chapter03_Application : public Base::Application
{
public:
    Chapter03_Application() : Application("Chapter 03: Camera"),
                              m_Camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

protected:
    void setup() override
    {
        LOG_INFO("Chapter 03 setup...");

        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile(
            "shaders/chapter03.vert",
            "shaders/chapter03.frag");

        m_Shader->use();
        m_PosAttribLocation = glGetAttribLocation(m_Shader->getProgramID(), "aPos");
        if (m_PosAttribLocation == -1)
        {
            LOG_ERROR("Could not find attribute 'aPos' in the shader!");
        }
        LOG_INFO("aPos location found: {}", m_PosAttribLocation);

        float vertices[] = {
            // positions
            -0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,

            -0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,

            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,

            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,

            -0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            0.5f,
            -0.5f,
            -0.5f,
            -0.5f,

            -0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            0.5f,
            -0.5f,
            0.5f,
            -0.5f,
        };

        glGenVertexArrays(1, &m_VaoID);
        glGenBuffers(1, &m_VboID);

        glBindVertexArray(m_VaoID);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(m_PosAttribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(m_PosAttribLocation);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_Camera.setProjection(45.0f, (float)getWidth() / (float)getHeight(), 0.1f, 100.0f);

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
        auto &input = Base::Input::Get();
        CameraInput currentFrameInput;

        if (input.IsKeyDown(SDL_SCANCODE_W))
            currentFrameInput.moveForward = -1.0f;
        if (input.IsKeyDown(SDL_SCANCODE_S))
            currentFrameInput.moveForward = 1.0f;
        if (input.IsKeyDown(SDL_SCANCODE_A))
            currentFrameInput.moveRight = -1.0f;
        if (input.IsKeyDown(SDL_SCANCODE_D))
            currentFrameInput.moveRight = 1.0f;
        if (input.IsKeyDown(SDL_SCANCODE_SPACE))
            currentFrameInput.moveUp = 1.0f;
        if (input.IsKeyDown(SDL_SCANCODE_LCTRL))
            currentFrameInput.moveUp = -1.0f;

        if (isViewportHovered() && input.IsMouseButtonPressed(SDL_BUTTON_LEFT))
        {
            input.SetRelativeMouseMode(true); // Capture mouse for relative movement
        }

        // If Escape is pressed, release the mouse.
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            input.SetRelativeMouseMode(false);
        }

        // Only process mouse look if relative mode is enabled (i.e., mouse is captured)
        if (input.IsRelativeMouseMode())
        {
            glm::vec2 mouseDelta = input.GetMouseDelta();
            currentFrameInput.mouseDeltaX = mouseDelta.x;
            currentFrameInput.mouseDeltaY = -mouseDelta.y; // Invert Y-axis
        }
        m_Camera.update(currentFrameInput, deltaTime);
    }

    void render() override
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Shader->use();
        m_Shader->setVec4("u_TriangleColor", glm::make_vec4(m_TriangleColor));

        glm::mat4 model = glm::mat4(1.0f);
        m_Shader->setMat4("model", model);
        m_Shader->setMat4("view", m_Camera.getViewMatrix());
        m_Shader->setMat4("projection", m_Camera.getProjectionMatrix());

        glBindVertexArray(m_VaoID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Chapter 02 Triangle & Uniforms");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Triangle Color", m_TriangleColor);
        ImGui::Separator();
        ImGui::Text("Camera Controls");
        
        // Movement Speed
        float speed = m_Camera.getMovementSpeed();
        if (ImGui::DragFloat("Speed", &speed, 0.1f, 0.1f, 100.0f))
        {
            m_Camera.setMovementSpeed(speed);
        }

        // Mouse Sensitivity
        float sensitivity = m_Camera.getMouseSensitivity();
        if (ImGui::DragFloat("Sensitivity", &sensitivity, 0.01f, 0.01f, 1.0f))
        {
            m_Camera.setMouseSensitivity(sensitivity);
        }

        ImGui::Separator();
        ImGui::Text("Projection Settings");

        // Field of View (FOV)
        float fov = m_Camera.getFov();
        if (ImGui::SliderFloat("Field of View", &fov, 1.0f, 120.0f))
        {
            m_Camera.setFov(fov);
        }

        // Near and Far Planes
        float nearPlane = m_Camera.getNearPlane();
        if (ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.01f, 100.0f))
        {
            m_Camera.setNearPlane(nearPlane);
        }

        float farPlane = m_Camera.getFarPlane();
        if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 1.0f, 1000.0f))
        {
            m_Camera.setFarPlane(farPlane);
        }

        ImGui::Separator();
        ImGui::Text("Camera State (Read-only)");

        // Display camera position
        glm::vec3 pos = m_Camera.getPosition();
        ImGui::InputFloat3("Position", &pos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

        // Display camera orientation
        float yaw = m_Camera.getYaw();
        float pitch = m_Camera.getPitch();
        ImGui::InputFloat("Yaw", &yaw, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Pitch", &pitch, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
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