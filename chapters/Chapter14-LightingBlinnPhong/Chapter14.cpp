#include "Chapter14.hpp"
#include "Log.hpp"
#include "Input.hpp"
#include "Application.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct CameraMatrices
{
    glm::mat4 view;
    glm::mat4 projection;
};

#ifdef BUILD_STANDALONE
Chapter14_Application::Chapter14_Application(std::string title, int width, int height)
    : ChapterBase(title, width, height)
{
    LOG_INFO("Chapter 14 constructed in STANDALONE mode.");
}
#else
Chapter14_Application::Chapter14_Application()
    : ChapterBase()
{
    LOG_INFO("Chapter 14 constructed in BUNDLED mode.");
}
#endif

void Chapter14_Application::setup()
{
    setupShaders();
    setupGeometry();
    setupCamera();
    setupEventListeners();
}

void Chapter14_Application::setupShaders()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/chapter14.vert", "shaders/chapter14.frag");
    unsigned int chapter14_UBO_Index = glGetUniformBlockIndex(m_Shader->getProgramID(), "CameraUBO");
    glUniformBlockBinding(m_Shader->getProgramID(), chapter14_UBO_Index, 0);

    m_LightCubeShader = std::make_unique<Base::Shader>();
    m_LightCubeShader->loadFromFile(
        "shaders/light_obj.vert",
        "shaders/light_obj.frag");
    unsigned int lightCube_UBO_Index = glGetUniformBlockIndex(m_LightCubeShader->getProgramID(), "CameraUBO");
    glUniformBlockBinding(m_LightCubeShader->getProgramID(), lightCube_UBO_Index, 0);

    m_GuideShader = std::make_unique<Base::Shader>();
    m_GuideShader->loadFromFile("shaders/guideMVP.vert", "shaders/guide.frag");
    unsigned int guide_UBO_Index = glGetUniformBlockIndex(m_GuideShader->getProgramID(), "CameraUBO");
    glUniformBlockBinding(m_GuideShader->getProgramID(), guide_UBO_Index, 0);
}

void Chapter14_Application::setupGeometry()
{
    setupCube();
    setupLightCube();
    setupCoordinateGuide();

    m_Texture = std::make_unique<Base::Texture>();
    m_Texture->loadFromFile("images/uv.png");
}

void Chapter14_Application::setupCamera()
{
    auto &app = Base::Application::getInstance();

    m_Camera.setPosition({0.0f, 0.0f, 3.0f});
    m_Camera.lookAt({0.0f, 0.0f, 0.0f});
    m_Camera.setProjection(45.0f, app.getViewportAspectRatio(), 0.1f, 100.0f);
    glGenBuffers(1, &m_CameraUboID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUboID);

    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraMatrices), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraUboID);
}

void Chapter14_Application::setupEventListeners()
{
    auto &app = Base::Application::getInstance();

    m_mouseButtonSub = app.getEventBus().subscribe<Base::MouseButtonPressedEvent>([this, &app](Base::MouseButtonPressedEvent &e)
                                                                                  {
        if (app.isViewportHovered() && e.button == SDL_BUTTON_LEFT) {
            e.handled = true;
        } });

    m_keyPressSub = app.getEventBus().subscribe<Base::KeyPressedEvent>([this, &app](Base::KeyPressedEvent &e)
                                                                       {
        if (e.key == SDLK_ESCAPE && !e.isRepeat) {
            Base::Input::Get().SetRelativeMouseMode(false);
            e.handled = true;
        } });
}

void Chapter14_Application::shutdown()
{
    auto& app = Base::Application::getInstance();
    app.getEventBus().unsubscribe(m_mouseButtonSub);
    app.getEventBus().unsubscribe(m_keyPressSub);

    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    glDeleteBuffers(1, &m_EboID);
    glDeleteVertexArrays(1, &m_GuideVaoID);
    glDeleteBuffers(1, &m_GuideVboID);
    glDeleteVertexArrays(1, &m_LightCubeVaoID);
    glDeleteBuffers(1, &m_CameraUboID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);

    m_Shader.reset();
    m_Texture.reset();
    m_GuideShader.reset();
    m_LightCubeShader.reset();
    glDisable(GL_CULL_FACE);

}

static CameraInput gatherInput()
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
        frameInput.mouseDeltaY = -mouseDelta.y;
    }
    return frameInput;
}

void Chapter14_Application::render()
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

    glEnable(GL_DEPTH_TEST);

    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CameraMatrices camData;
    camData.view = m_Camera.getViewMatrix();
    camData.projection = m_Camera.getProjectionMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUboID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraMatrices), &camData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_Shader->use();
    m_Shader->setMat4("model", m_ModelMatrix);

    m_Shader->setMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix))));
    m_Shader->setVec3("u_LightPos", m_LightPos);
    m_Shader->setVec3("u_ViewPos", m_Camera.getPosition());
    m_Shader->setVec4("u_LightColor", glm::make_vec4(m_LightColor));
    m_Shader->setInt("u_Texture", 0);
    m_Shader->setBool("u_UseTexture", m_UseTexture);
    m_Shader->setVec4("u_TintColor", glm::make_vec4(m_TintColor));

    m_Texture->bind(0);
    glBindVertexArray(m_VaoID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    m_LightCubeShader->use();
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, m_LightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));

    m_LightCubeShader->setMat4("model", lightModel);
    m_LightCubeShader->setVec4("u_ObjectColor", glm::make_vec4(m_LightColor));

    glBindVertexArray(m_LightCubeVaoID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    if (m_ShowCoordinateGuide)
    {
        m_GuideShader->use();
        m_GuideShader->setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(m_GuideVaoID);
        glDrawArrays(GL_LINES, 0, 6);
        glLineWidth(1.0f);
    }

    glBindVertexArray(0);
}

void Chapter14_Application::renderChapterUI()
{
    ImGui::Begin("Settings");
    drawSceneSettingsUI();
    drawLightSettingsUI();
    drawCubeTransformUI();
    drawCameraSettingsUI();
    drawCullingSettingsUI();
    ImGui::End();

    drawMouseCapturePopup();
}

void Chapter14_Application::drawSceneSettingsUI()
{
    if (ImGui::CollapsingHeader("Scene"))
    {
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::Checkbox("Show Coordinate Guide", &m_ShowCoordinateGuide);
    }
}

void Chapter14_Application::drawLightSettingsUI()
{
    if (ImGui::CollapsingHeader("Light Settings"))
    {
        ImGui::DragFloat3("Light Position", &m_LightPos.x, 0.01f);
        ImGui::ColorEdit4("Light Color", m_LightColor);
    }
}

void Chapter14_Application::drawCubeTransformUI()
{
    if (ImGui::CollapsingHeader("Cube Transformation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat3("Position", &m_Position.x, 0.01f);
        ImGui::DragFloat3("Rotation", &m_RotationEuler.x, 1.0f, -180.0f, 180.0f);
        ImGui::DragFloat3("Scale", &m_Scale.x, 0.01f);
        ImGui::ColorEdit4("Tint Color", m_TintColor);
        ImGui::Checkbox("Use Texture", &m_UseTexture);
        if (ImGui::Button("Reset Cube"))
        {
            m_Position = glm::vec3(0.0f);
            m_RotationEuler = glm::vec3(0.0f);
            m_Scale = glm::vec3(1.0f);
        }
    }
}

void Chapter14_Application::drawCameraSettingsUI()
{
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
}

void Chapter14_Application::drawCullingSettingsUI()
{
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
}

void Chapter14_Application::handleInput(float deltaTime)
{
}

void Chapter14_Application::update(float deltaTime)
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

void Chapter14_Application::setupCube()
{
    //clang-format off
    Vertex vertices[] = {
        // positions          // normals           // texture Coords
        // Front Face (+Z) - Normal (0, 0, 1)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // 2
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3

        // Back Face (-Z) - Normal (0, 0, -1) -- CORRECTED
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 4
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // 5
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},   // 6
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 7

        // Left Face (-X) - Normal (-1, 0, 0)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 8
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 9
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // 10
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 11

        // Right Face (+X) - Normal (1, 0, 0) -- CORRECTED
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 12
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 13
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // 14
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 15

        // Top Face (+Y) - Normal (0, 1, 0)
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 17
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 18
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 19

        // Bottom Face (-Y) - Normal (0, -1, 0)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 20
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // 21
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},   // 22
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}   // 23
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
    //
    //clang-format on

    glGenVertexArrays(1, &m_VaoID);
    glGenBuffers(1, &m_VboID);
    glGenBuffers(1, &m_EboID);

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const GLsizei stride = sizeof(Vertex);

    // layout (location = 0) in vec3 aPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // layout (location = 1) in vec3 aNormal;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // layout (location = 2) in vec2 aTexCoord;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chapter14_Application::setupLightCube()
{
    glGenVertexArrays(1, &m_LightCubeVaoID);
    glBindVertexArray(m_LightCubeVaoID);

    // Reuse the VBO and EBO from the main cube
    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);

    const GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Chapter14_Application::setupCoordinateGuide()
{
    
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

void Chapter14_Application::drawMouseCapturePopup()
{
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

    if (!Base::Input::Get().IsRelativeMouseMode())
    {
        if (ImGui::Begin("MouseCapturePopup", nullptr, flags))
        {
            ImGui::Text("Press Right-click to capture mouse");
        }
        ImGui::End();
        return;
    }
    else
    {
        if (ImGui::Begin("MouseCapturePopup", nullptr, flags))
        {
            ImGui::Text("Press ESC to release mouse");
        }
        ImGui::End();
    }
}