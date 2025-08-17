#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "EventTypes.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Texture.hpp"

#include <memory>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>

class Chapter06_Application : public Base::Application
{
public:
    Chapter06_Application() : Application("Chapter 05: Camera") {}

protected:
    void setup() override
    {
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile(
            "shaders/chapter05.vert",
            "shaders/chapter05.frag");

        m_GuideShader = std::make_unique<Base::Shader>();
        m_GuideShader->loadFromFile("shaders/guide.vert", "shaders/guide.frag");

        setupCube();
        setupCoordinateGuide();

        m_Texture = std::make_unique<Base::Texture>();
        m_Texture->loadFromFile("assets/images/uv.png");

        m_Camera.setPosition({0.0f, 0.0f, 3.0f});
        m_Camera.lookAt({0.0f, 0.0f, 0.0f});
        m_Camera.setProjection(45.0f, getViewportAspectRatio(), 0.1f, 100.0f);

        subscribeToMouseButtons([this](Base::MouseButtonPressedEvent &e)
                                {
            if (isViewportHovered() && e.button == SDL_BUTTON_LEFT) {
                ImGui::ClearActiveID();

                ImGui::SetWindowFocus("Viewport");
                Base::Input::Get().SetRelativeMouseMode(true);
                SDL_GetRelativeMouseState(nullptr, nullptr);
                e.handled = true;
            } });

        subscribeToKeys([this](Base::KeyPressedEvent &e)
                        {
            if (e.key == SDLK_ESCAPE && !e.isRepeat) {
                Base::Input::Get().SetRelativeMouseMode(false);
                e.handled = true;
            } });
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &m_VaoID);
        glDeleteBuffers(1, &m_VboID);
        glDeleteBuffers(1, &m_EboID);
        glDeleteVertexArrays(1, &m_GuideVaoID);
        glDeleteBuffers(1, &m_GuideVboID);
        m_Shader.reset();
        m_Texture.reset();
        m_GuideShader.reset();
    }

    void handleInput(float deltaTime) override
    {
    }

    CameraInput gatherInput()
    {
        auto &input = Base::Input::Get();
        CameraInput frameInput; // A temporary, local struct

        if (!ImGui::GetIO().WantCaptureKeyboard || input.IsRelativeMouseMode())
        {
            if (input.IsKeyDown(SDL_SCANCODE_W))
                frameInput.moveForward += 1.0f;
            if (input.IsKeyDown(SDL_SCANCODE_S))
                frameInput.moveForward -= 1.0f;
            if (input.IsKeyDown(SDL_SCANCODE_A))
                frameInput.moveRight -= 1.0f;
            if (input.IsKeyDown(SDL_SCANCODE_D))
                frameInput.moveRight += 1.0f;
            if (input.IsKeyDown(SDL_SCANCODE_SPACE))
                frameInput.moveUp += 1.0f;
            if (input.IsKeyDown(SDL_SCANCODE_LCTRL))
                frameInput.moveUp -= 1.0f;
        }

        if (input.IsRelativeMouseMode())
        {
            glm::vec2 mouseDelta = input.GetMouseDelta();
            frameInput.mouseDeltaX = mouseDelta.x;
            frameInput.mouseDeltaY = -mouseDelta.y; // TODO: Add imGui support for this
        }
        return frameInput;
    }

    void update(float deltaTime) override
    {
        CameraInput currentFrameInput = gatherInput();
        m_Camera.update(currentFrameInput, deltaTime);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        model = glm::rotate(model, glm::radians(m_RotationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_RotationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_RotationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, m_Scale);
        m_ModelMatrix = model;
    }

    void render() override
    {
        if (m_FaceCullingEnabled)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_CullFaceMode == 0 ? GL_BACK : GL_FRONT);
            glFrontFace(m_WindingOrderMode == 0 ? GL_CCW : GL_CW);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        // Depth Testing
        glEnable(GL_DEPTH_TEST);

        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = m_Camera.getViewMatrix();
        glm::mat4 projection = m_Camera.getProjectionMatrix();

        // --- Draw the Cube ---
        m_Shader->use();
        m_Shader->setMat4("model", m_ModelMatrix);
        m_Shader->setMat4("view", view);
        m_Shader->setMat4("projection", projection);
        m_Shader->setInt("u_Texture", 0);
        m_Shader->setVec4("u_TintColor", glm::make_vec4(m_TintColor));
        m_Texture->bind(0);
        glBindVertexArray(m_VaoID);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        if (m_ShowCoordinateGuide)
        {
            m_GuideShader->use();
            m_GuideShader->setMat4("model", glm::mat4(1.0f));
            m_GuideShader->setMat4("view", view);
            m_GuideShader->setMat4("projection", projection);
            glLineWidth(2.5f);
            glBindVertexArray(m_GuideVaoID);
            glDrawArrays(GL_LINES, 0, 6);
            glLineWidth(1.0f);
        }

        glBindVertexArray(0);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Settings");

        if (ImGui::CollapsingHeader("Scene"))
        {
            ImGui::ColorEdit3("Background Color", m_ClearColor);
            ImGui::Checkbox("Show Coordinate Guide", &m_ShowCoordinateGuide);
        }

        if (ImGui::CollapsingHeader("Cube Transformation", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Position", &m_Position.x, 0.01f);
            ImGui::DragFloat3("Rotation", &m_RotationEuler.x, 1.0f, -180.0f, 180.0f);
            ImGui::DragFloat3("Scale", &m_Scale.x, 0.01f);
            ImGui::ColorEdit4("Tint Color", m_TintColor);
            if (ImGui::Button("Reset Cube"))
            {
                m_Position = glm::vec3(0.0f);
                m_RotationEuler = glm::vec3(0.0f);
                m_Scale = glm::vec3(1.0f);
            }
        }

        if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextWrapped("Use WASD + Space/LCTRL to move the camera.\n"
                               "Mouse movement is relative when in 'Relative Mouse Mode'.\n"
                               "Press ESC to exit Relative Mouse Mode.");
            ImGui::SeparatorText("Camera Mode");

            int currentMode = static_cast<int>(m_Camera.getMode());

            if (ImGui::RadioButton("FPS", &currentMode, static_cast<int>(CameraMode::FPS)))
            {
                m_Camera.setMode(CameraMode::FPS);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Fly", &currentMode, static_cast<int>(CameraMode::FLY)))
            {
                m_Camera.setMode(CameraMode::FLY);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("FPS: Horizontal movement is locked to the XZ plane.\n"
                                  "Fly: Full 6 degrees of freedom movement.");
            }
            float speed = m_Camera.getMovementSpeed();
            if (ImGui::DragFloat("Speed", &speed, 0.1f, 0.1f, 100.0f))
            {
                m_Camera.setMovementSpeed(speed);
            }
            float sensitivity = m_Camera.getMouseSensitivity();
            if (ImGui::DragFloat("Sensitivity", &sensitivity, 0.01f, 0.01f, 1.0f))
            {
                m_Camera.setMouseSensitivity(sensitivity);
            }
            float fov = m_Camera.getFov();
            if (ImGui::SliderFloat("Field of View", &fov, 1.0f, 120.0f))
            {
                m_Camera.setFov(fov);
            }
            glm::vec3 camPos = m_Camera.getPosition();
            ImGui::InputFloat3("Position (Read-Only)", &camPos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
        }

        if (ImGui::CollapsingHeader("Culling Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Enable Face Culling", &m_FaceCullingEnabled);

            ImGui::BeginDisabled(!m_FaceCullingEnabled);
            {
                ImGui::SeparatorText("Face to Cull");
                ImGui::RadioButton("Back", &m_CullFaceMode, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Front", &m_CullFaceMode, 1);

                ImGui::SeparatorText("Front Face Winding Order");
                ImGui::RadioButton("Counter-Clockwise (CCW)", &m_WindingOrderMode, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Clockwise (CW)", &m_WindingOrderMode, 1);

                ImGui::TextWrapped("NOTE: The cube's vertices are defined in CCW order. "
                                   "Culling BACK faces with CCW winding is the standard setting.");
            }
            ImGui::EndDisabled();
        }

        ImGui::End();

        drawMouseCapturePopup();
    }
    Camera *getActiveCamera() override { return &m_Camera; }

private:
    // Cube Objects
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;
    GLuint m_VaoID = 0, m_VboID = 0, m_EboID = 0;
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_RotationEuler = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    float m_TintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Guide Objects
    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;

    // Camera Objects
    Camera m_Camera;

    // Scene Objects
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    // Face Culling Settings
    bool m_FaceCullingEnabled = true;
    int m_CullFaceMode = 0;     // 0 for GL_BACK, 1 for GL_FRONT
    int m_WindingOrderMode = 0; // 0 for GL_CCW, 1 for GL_CW

    void setupCube()
    {
        //clang-format off
        float vertices[] = {
            // positions          // texture Coords
            // Front Face (+Z)
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // 0
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // 1
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // 2
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // 3

            // Back Face (-Z)
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // 4
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  // 5
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   // 6
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 7

            // Left Face (-X)
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 8
            -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // 9
            -0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // 10
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // 11

            // Right Face (+X)
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // 12
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // 13
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 14
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f,   // 15

            // Top Face (+Y)
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,  // 16
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 17
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 18
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // 19

            // Bottom Face (-Y)
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 20
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // 21
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f,   // 22
            -0.5f, -0.5f, 0.5f, 0.0f, 1.0f   // 23
        };

        // Index pattern 0, 1, 2 and 2, 3, 0 is CCW for the vertex order above.
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,       // Front
            5, 4, 7, 7, 6, 5,       // Back  (Note: Flipped order 5,4,7 to be CCW from outside)
            8, 9, 10, 10, 11, 8,    // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom (Note: Flipped order to be CCW from outside)
        };

        //clang-format on

        glGenVertexArrays(1, &m_VaoID);
        glGenBuffers(1, &m_VboID);
        glGenBuffers(1, &m_EboID);

        glBindVertexArray(m_VaoID);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void setupCoordinateGuide()
    {
        m_GuideShader = std::make_unique<Base::Shader>();
        m_GuideShader->loadFromFile("shaders/guideMVP.vert", "shaders/guide.frag");
        float guideVertices[] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
        glGenVertexArrays(1, &m_GuideVaoID);
        glGenBuffers(1, &m_GuideVboID);
        glBindVertexArray(m_GuideVaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_GuideVboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(guideVertices), guideVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void drawMouseCapturePopup()
    {
        if (!Base::Input::Get().IsRelativeMouseMode())
        {
            return;
        }

        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |       // No title bar, borders, or resize grips
                                       ImGuiWindowFlags_NoMove |             // Can't be dragged
                                       ImGuiWindowFlags_AlwaysAutoResize |   // Shrink-wrap to content
                                       ImGuiWindowFlags_NoSavedSettings |    // Don't save position in imgui.ini
                                       ImGuiWindowFlags_NoFocusOnAppearing | // Don't steal focus
                                       ImGuiWindowFlags_NoNav;               // Disable keyboard/gamepad navigation

        const float padding = 10.0f;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos = ImVec2(work_pos.x + work_size.x * 0.5f, work_pos.y + work_size.y - padding);
        ImVec2 window_pivot = ImVec2(0.5f, 1.0f);

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pivot);
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("MouseCapturePopup", nullptr, flags))
        {
            ImGui::Text("Press ESC to release mouse");
        }
        ImGui::End();
    }
};

int main(int argc, char *argv[])
{
    Logger::getInstance().initialize({});

    try
    {
        auto app = std::make_unique<Chapter06_Application>();
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