#include "ShaderEditor.hpp"
#include "Chapter19.hpp"
#include "Log.hpp"
#include "Input.hpp"
#include "Application.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace
{
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
}

struct Light
{
    glm::vec4 position;  // Using vec4 for alignment. For directional, w=0.
    glm::vec4 direction; // Using vec4 for alignment. For point, unused.
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec4 attenuation; // (constant, linear, quadratic, unused)
    glm::vec4 spotParams;  // (cutOff, outerCutOff, type, unused)
};

#ifdef BUILD_STANDALONE
Chapter19_Application::Chapter19_Application(std::string title, int width, int height)
    : ChapterBase(title, width, height)
{
    LOG_INFO("Chapter 15 constructed in STANDALONE mode.");
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        m_pLight[i] = std::make_unique<Light>();
    }
}
#else
Chapter19_Application::Chapter19_Application()
    : ChapterBase()
{
    LOG_INFO("Chapter 15 constructed in BUNDLED mode.");
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        m_pLight[i] = std::make_unique<Light>();
    }
}
#endif
Chapter19_Application::~Chapter19_Application() = default;

void Chapter19_Application::setup()
{
    Base::Application::getInstance().setGridEnabled(m_showGrid);
    m_MaterialPresets = g_MaterialPresets;
    setupScene();
    setupShaders();
    setupGeometry();
    setupSkybox(); 
    setupCamera();
    setupEventListeners();
}

void Chapter19_Application::setupScene()
{
    // Position some cubes in the world
    m_CubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    m_CubePositions.push_back(glm::vec3(2.0f, 5.0f, -15.0f));
    m_CubePositions.push_back(glm::vec3(-1.5f, -2.2f, -2.5f));
    m_CubePositions.push_back(glm::vec3(-3.8f, -2.0f, -12.3f));
    m_CubePositions.push_back(glm::vec3(2.4f, -0.4f, -3.5f));
    m_CubePositions.push_back(glm::vec3(-1.7f, 3.0f, -7.5f));
    m_CubePositions.push_back(glm::vec3(1.3f, -2.0f, -2.5f));
    m_CubePositions.push_back(glm::vec3(1.5f, 2.0f, -2.5f));
    m_CubePositions.push_back(glm::vec3(1.5f, 0.2f, -1.5f));
    m_CubePositions.push_back(glm::vec3(-1.3f, 1.0f, -1.5f));

    srand(1337); // for consistent random colors
    // Light 0: A white directional light
    m_pLight[0]->position = glm::vec4(0.0f);
    m_pLight[0]->direction = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
    m_pLight[0]->ambient = glm::vec4(0.05f);
    m_pLight[0]->diffuse = glm::vec4(0.3f);
    m_pLight[0]->specular = glm::vec4(0.5f);
    m_pLight[0]->attenuation = glm::vec4(1.0f, 0, 0, 0);
    m_pLight[0]->spotParams = glm::vec4(0, 0, static_cast<float>(LightType::Directional), 0);

    // Light 1: The camera-attached flashlight (spotlight)
    m_pLight[1]->position = glm::vec4(m_Camera.getPosition(), 1.0f);
    m_pLight[1]->direction = glm::vec4(m_Camera.getFront(), 0.0f);
    m_pLight[1]->ambient = glm::vec4(0.0f);
    m_pLight[1]->diffuse = glm::vec4(1.0f);
    m_pLight[1]->specular = glm::vec4(1.0f);
    m_pLight[1]->attenuation = glm::vec4(1.0f, 0.09f, 0.032f, 0);
    m_pLight[1]->spotParams = glm::vec4(glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), static_cast<float>(LightType::Spot), 1.0f); // is_flashlight = 1

    // Lights 2 through N: Random colored point lights
    for (int i = 2; i < MAX_LIGHTS; ++i)
    {
        m_pLight[i]->position = glm::vec4(sin(i * 3.14f) * 6, cos(i * 1.77f) * 2, cos(i * 5.2f) * 6, 1.0f);
        m_pLight[i]->direction = glm::vec4(0.0f);
        m_pLight[i]->ambient = glm::vec4(0.05f);
        m_pLight[i]->diffuse = glm::vec4((rand() % 100) / 200.0f + 0.5f, (rand() % 100) / 200.0f + 0.5f, (rand() % 100) / 200.0f + 0.5f, 1.0f);
        m_pLight[i]->specular = glm::vec4(1.0f);
        m_pLight[i]->attenuation = glm::vec4(1.0f, 0.09f, 0.032f, 0);
        m_pLight[i]->spotParams = glm::vec4(0, 0, static_cast<float>(LightType::Point), 0);
    }
}

void Chapter19_Application::setupShaders()
{
    m_Shader = std::make_unique<Base::Shader>();
    m_Shader->loadFromFile("shaders/Chapter19.vert", "shaders/Chapter19.frag");

    m_LightCubeShader = std::make_unique<Base::Shader>();
    m_LightCubeShader->loadFromFile("shaders/light_obj.vert", "shaders/light_obj.frag");

    m_GuideShader = std::make_unique<Base::Shader>();
    m_GuideShader->loadFromFile("shaders/guideMVP.vert", "shaders/guide.frag");

    rebindShaderUniformBlocks();

    Base::ShaderEditor::getInstance().registerShader("Main Cube Shader", m_Shader.get());

    unsigned int mainShader_UBO_Index = glGetUniformBlockIndex(m_Shader->getProgramID(), "CameraUBO");
    glUniformBlockBinding(m_Shader->getProgramID(), mainShader_UBO_Index, 0);
    Base::ShaderEditor::getInstance().registerShader("Light Cube Shader", m_LightCubeShader.get());

    unsigned int lightCube_UBO_Index = glGetUniformBlockIndex(m_LightCubeShader->getProgramID(), "CameraUBO");
    glUniformBlockBinding(m_LightCubeShader->getProgramID(), lightCube_UBO_Index, 0);
    Base::ShaderEditor::getInstance().registerShader("Guide Shader", m_GuideShader.get());

    unsigned int guide_UBO_Index = glGetUniformBlockIndex(m_GuideShader->getProgramID(), "CameraUBO");
    glUniformBlockBinding(m_GuideShader->getProgramID(), guide_UBO_Index, 0);

    unsigned int lightsUBO_Index = glGetUniformBlockIndex(m_Shader->getProgramID(), "LightsUBO");
    glUniformBlockBinding(m_Shader->getProgramID(), lightsUBO_Index, 1);

    glGenBuffers(1, &m_LightsUboID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_LightsUboID);

    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * MAX_LIGHTS, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_LightsUboID);
}

void Chapter19_Application::setupGeometry()
{
    setupCube();
    setupLightCube();
    setupCoordinateGuide();
}

void Chapter19_Application::setupCamera()
{
    auto &app = Base::Application::getInstance();

    m_Camera.setPosition({0.0f, 0.0f, 3.0f});
    m_Camera.lookAt({0.0f, 0.0f, 0.0f});

    glGenBuffers(1, &m_CameraUboID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUboID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraMatrices), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraUboID);
}

void Chapter19_Application::setupEventListeners()
{
    auto &app = Base::Application::getInstance();
    m_mouseButtonSub = app.getEventBus().subscribe<Base::MouseButtonPressedEvent>([this, &app](Base::MouseButtonPressedEvent &e)
                                                                                  {
        if (app.isViewportHovered() && e.button == SDL_BUTTON_LEFT) {
            e.handled = true;
        } });

    m_keyPressSub = app.getEventBus().subscribe<Base::KeyPressedEvent>([this](Base::KeyPressedEvent &e)
                                                                       {
        if (e.key == SDLK_ESCAPE && !e.isRepeat) {
            Base::Input::Get().SetRelativeMouseMode(false);
            e.handled = true;
        } });
}

void Chapter19_Application::shutdown()
{
    auto &app = Base::Application::getInstance();
    app.setGridEnabled(false);

    app.getEventBus().unsubscribe(m_mouseButtonSub);
    app.getEventBus().unsubscribe(m_keyPressSub);

    
    glDeleteVertexArrays(1, &m_VaoID);
    glDeleteBuffers(1, &m_VboID);
    glDeleteBuffers(1, &m_EboID);
    glDeleteVertexArrays(1, &m_GuideVaoID);
    glDeleteBuffers(1, &m_GuideVboID);
    glDeleteVertexArrays(1, &m_LightCubeVaoID);
    glDeleteBuffers(1, &m_CameraUboID);
    glDeleteVertexArrays(1, &m_skyboxVao);
    glDeleteBuffers(1, &m_skyboxVbo);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);

    m_Shader.reset();
    m_DiffuseTexture.reset();
    m_SpecularTexture.reset();
    m_GuideShader.reset();
    m_LightCubeShader.reset();
    m_skyboxShader.reset();
    m_skyboxTexture.reset();

    glDisable(GL_CULL_FACE);
}

static CameraInput gatherInput()
{
    auto &input = Base::Input::Get();
    CameraInput frameInput;

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

void Chapter19_Application::render()
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

    CameraMatrices camData = {m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix()};
    glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUboID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraMatrices), &camData);

    glBindBuffer(GL_UNIFORM_BUFFER, m_LightsUboID);
    std::vector<Light> lightData;
    lightData.reserve(m_ActiveLights);
    for (int i = 0; i < m_ActiveLights; ++i)
    {
        lightData.push_back(*m_pLight[i]);
    }
    if (!lightData.empty())
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * lightData.size(), lightData.data());
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_Shader->use();

    m_Shader->setVec3("u_ViewPos", m_Camera.getPosition());
    m_Shader->setInt("u_ActiveLights", m_ActiveLights);

    const auto &currentPreset = m_MaterialPresets[m_CurrentMaterialIndex];
    m_Shader->setFloat("material.shininess", currentPreset.Shininess);
    m_Shader->setVec4("material.tintColor", glm::make_vec4(m_TintColor));
    m_Shader->setInt("material.useTexture", m_UseTexture ? 1 : 0);
    m_Shader->setVec3("material.baseDiffuse", currentPreset.Diffuse);
    m_Shader->setVec3("material.baseSpecular", currentPreset.Specular);

    m_Shader->setInt("u_DiffuseMap", 0);
    m_Shader->setInt("u_SpecularMap", 1);

    m_DiffuseTexture->bind(0);
    m_SpecularTexture->bind(1);
    glBindVertexArray(m_VaoID);

    m_Shader->setMat4("model", m_ModelMatrix);
    m_Shader->setMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix))));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    for (const auto &pos : m_CubePositions)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        m_Shader->setMat4("model", model);
        m_Shader->setMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    m_LightCubeShader->use();
    glBindVertexArray(m_LightCubeVaoID);
    for (int i = 0; i < m_ActiveLights; ++i)
    {
        if (m_pLight[i]->spotParams.z > 0.5f)
        {
            glm::mat4 lightModel(1.0f);
            lightModel = glm::translate(lightModel, glm::vec3(m_pLight[i]->position));
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            m_LightCubeShader->setMat4("model", lightModel);
            m_LightCubeShader->setVec4("u_ObjectColor", m_pLight[i]->diffuse);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }

    if (m_ShowCoordinateGuide)
    {
        m_GuideShader->use();
        m_GuideShader->setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(m_GuideVaoID);
        glDrawArrays(GL_LINES, 0, 6);
    }

    glBindVertexArray(0);

    if (m_showSkybox)
    {
        renderSkybox();
    }
}

void Chapter19_Application::renderChapterUI()
{
    ImGui::Begin("Settings");

    if (ImGui::CollapsingHeader("Scene"))
    {
        if (ImGui::Checkbox("Show Grid", &m_showGrid))
        {
            Base::Application::getInstance().setGridEnabled(m_showGrid);
        }
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::Checkbox("Show Skybox", &m_showSkybox);
        ImGui::Checkbox("Show Coordinate Guide", &m_ShowCoordinateGuide);
    }

    if (ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderInt("Active Lights", &m_ActiveLights, 0, MAX_LIGHTS);
        ImGui::Separator();

        if (m_ActiveLights > 0)
        {

            ImGui::SliderInt("Edit Light Index", &m_EditedLightIndex, 0, m_ActiveLights - 1);
            ImGui::Separator();
            Light &light = *m_pLight[m_EditedLightIndex]; // Get a reference to the light being edited
            // Create a unique ID for the UI elements to prevent conflicts
            char node_id[32];
            snprintf(node_id, sizeof(node_id), "Light %d Settings", m_EditedLightIndex);

            if (ImGui::TreeNode(node_id))
            {
                int type = static_cast<int>(light.spotParams.z);
                ImGui::RadioButton("Directional", &type, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Point", &type, 1);
                ImGui::SameLine();
                ImGui::RadioButton("Spotlight", &type, 2);
                light.spotParams.z = static_cast<float>(type);

                // UI for Directional Light
                if (type == 0)
                {
                    ImGui::DragFloat3("Direction", &light.direction.x, 0.01f);
                }
                // UI for Point Light
                if (type == 1)
                {
                    ImGui::DragFloat3("Position", &light.position.x, 0.01f);
                }
                // UI for Spot Light
                if (type == 2)
                {
                    bool is_flashlight = light.spotParams.w > 0.5f;
                    if (ImGui::Checkbox("Is Flashlight", &is_flashlight))
                    {
                        light.spotParams.w = is_flashlight ? 1.0f : 0.0f;
                    }

                    ImGui::BeginDisabled(is_flashlight);
                    ImGui::DragFloat3("Position##spot", &light.position.x, 0.01f);
                    ImGui::DragFloat3("Direction##spot", &light.direction.x, 0.01f);
                    ImGui::EndDisabled();

                    float cutOff_deg = glm::degrees(glm::acos(light.spotParams.x));
                    float outerCutOff_deg = glm::degrees(glm::acos(light.spotParams.y));
                    ImGui::SliderFloat("Inner CutOff Angle", &cutOff_deg, 0.0f, outerCutOff_deg);
                    ImGui::SliderFloat("Outer CutOff Angle", &outerCutOff_deg, cutOff_deg, 90.0f);
                    light.spotParams.x = glm::cos(glm::radians(cutOff_deg));
                    light.spotParams.y = glm::cos(glm::radians(outerCutOff_deg));
                }

                // UI for attenuation (Point and Spot)
                if (type > 0)
                {
                    ImGui::DragFloat3("Attenuation (C, L, Q)", &light.attenuation.x, 0.001f, 0.0f, 1.0f, "%.4f");
                }

                ImGui::ColorEdit3("Ambient", &light.ambient.x);
                ImGui::ColorEdit3("Diffuse", &light.diffuse.x);
                ImGui::ColorEdit3("Specular", &light.specular.x);
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Material Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Use Texture", &m_UseTexture);
        ImGui::Separator();

        if (ImGui::BeginCombo("Preset", m_MaterialPresets[m_CurrentMaterialIndex].Name))
        {
            for (int i = 0; i < m_MaterialPresets.size(); ++i)
            {
                const bool isSelected = (m_CurrentMaterialIndex == i);
                if (ImGui::Selectable(m_MaterialPresets[i].Name, isSelected))
                {
                    m_CurrentMaterialIndex = i;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        auto &currentMaterial = m_MaterialPresets[m_CurrentMaterialIndex];
        ImGui::BeginDisabled(m_UseTexture);
        {
            ImGui::ColorEdit3("Diffuse##material", &currentMaterial.Diffuse.x);
            ImGui::ColorEdit3("Specular##material", &currentMaterial.Specular.x);
        }
        ImGui::EndDisabled();
        ImGui::DragFloat("Shininess##material", &currentMaterial.Shininess, 0.1f, 0.0f, 256.0f);
    }

    if (ImGui::CollapsingHeader("Cube Transformation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat3("Position##cube", &m_Position.x, 0.01f);
        ImGui::DragFloat3("Rotation##cube", &m_RotationEuler.x, 1.0f, -180.0f, 180.0f);
        ImGui::DragFloat3("Scale##cube", &m_Scale.x, 0.01f);
        ImGui::ColorEdit4("Tint Color##cube", m_TintColor);
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
        ImGui::SeparatorText("Face to Cull");
        ImGui::RadioButton("Back", &m_CullFaceMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Front", &m_CullFaceMode, 1);
        ImGui::SeparatorText("Front Face Winding Order");
        ImGui::RadioButton("Counter-Clockwise (CCW)", &m_WindingOrderMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Clockwise (CW)", &m_WindingOrderMode, 1);
        ImGui::EndDisabled();
    }

    ImGui::End();
    drawMouseCapturePopup();
}

void Chapter19_Application::handleInput(float deltaTime) {}

void Chapter19_Application::update(float deltaTime)
{
    bool reloaded = false;
    if (m_Shader && m_Shader->tryReload())
        reloaded = true;
    if (m_LightCubeShader && m_LightCubeShader->tryReload())
        reloaded = true;
    if (m_GuideShader && m_GuideShader->tryReload())
        reloaded = true;

    if (reloaded)
    {
        rebindShaderUniformBlocks();
    }

    auto &app = Base::Application::getInstance();
    float currentAspectRatio = app.getViewportAspectRatio();
    m_Camera.setProjection(m_Camera.getFov(), currentAspectRatio, 0.1f, 100.0f);

    CameraInput currentFrameInput = gatherInput();
    m_Camera.update(currentFrameInput, deltaTime);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::rotate(model, glm::radians(m_RotationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_RotationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_RotationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, m_Scale);
    m_ModelMatrix = model;

    for (int i = 0; i < m_ActiveLights; ++i)
    {
        if (m_pLight[i]->spotParams.w > 0.5f)
        {
            m_pLight[i]->position = glm::vec4(m_Camera.getPosition(), 1.0f);
            m_pLight[i]->direction = glm::vec4(m_Camera.getFront(), 0.0f);
        }
    }
}

void Chapter19_Application::setupCube()
{
    m_DiffuseTexture = std::make_unique<Base::Texture>();
    m_DiffuseTexture->loadFromFile("images/cube_DefaultMaterial_Diffuse.png");

    m_SpecularTexture = std::make_unique<Base::Texture>();
    m_SpecularTexture->loadFromFile("images/cube_DefaultMaterial_SpecularGlossiness.png");

    Vertex vertices[] = {
        // positions          // normals           // texture Coords (U, V)

        // Front Face (+Z)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.00f, 0.375f}}, // Bottom-left
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.25f, 0.375f}},  // Bottom-right
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.25f, 0.625f}},   // Top-right
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.00f, 0.625f}},  // Top-left

        // Back Face (-Z)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.75f, 0.375f}}, // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.50f, 0.375f}},  // Bottom-right
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.50f, 0.625f}},   // Top-right
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.75f, 0.625f}},  // Top-left

        // Left Face (-X)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.75f, 0.375f}}, // Bottom-left
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.00f, 0.375f}},  // Bottom-right
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.00f, 0.625f}},   // Top-right
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.75f, 0.625f}},  // Top-left

        // Right Face (+X)
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.25f, 0.375f}},  // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.50f, 0.375f}}, // Bottom-right
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.50f, 0.625f}},  // Top-right
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.25f, 0.625f}},   // Top-left

        // Top Face (+Y) - Rotated 90 degrees CLOCKWISE
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.25f, 0.875f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.25f, 0.625f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.50f, 0.625f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.50f, 0.875f}},

        // Bottom Face (-Y) - Rotated 90 degrees COUNTER-CLOCKWISE
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.25f, 0.125f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.25f, 0.375f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.50f, 0.375f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.50f, 0.125f}}};

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, 5, 4, 7, 7, 6, 5, 8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

    glGenVertexArrays(1, &m_VaoID);
    glGenBuffers(1, &m_VboID);
    glGenBuffers(1, &m_EboID);

    glBindVertexArray(m_VaoID);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Chapter19_Application::setupLightCube()
{
    glGenVertexArrays(1, &m_LightCubeVaoID);
    glBindVertexArray(m_LightCubeVaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
    const GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Chapter19_Application::setupCoordinateGuide()
{
    float guideVertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    glGenVertexArrays(1, &m_GuideVaoID);
    glGenBuffers(1, &m_GuideVboID);
    glBindVertexArray(m_GuideVaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_GuideVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(guideVertices), guideVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Chapter19_Application::drawMouseCapturePopup()
{
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 window_pos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, viewport->WorkPos.y + viewport->WorkSize.y - 10.0f);
    ImVec2 window_pivot = ImVec2(0.5f, 1.0f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pivot);
    ImGui::SetNextWindowBgAlpha(0.35f);

    if (ImGui::Begin("MouseCapturePopup", nullptr, flags))
    {
        if (!Base::Input::Get().IsRelativeMouseMode())
        {
            ImGui::Text("Press Right-click to capture mouse");
        }
        else
        {
            ImGui::Text("Press ESC to release mouse");
        }
    }
    ImGui::End();
}

void Chapter19_Application::rebindShaderUniformBlocks()
{
    LOG_INFO("Re-binding Uniform Blocks for all shaders...");

    // Re-bind for the main shader
    if (m_Shader && m_Shader->getProgramID() != 0)
    {
        unsigned int mainShader_UBO_Index = glGetUniformBlockIndex(m_Shader->getProgramID(), "CameraUBO");
        glUniformBlockBinding(m_Shader->getProgramID(), mainShader_UBO_Index, 0);

        unsigned int lightsUBO_Index = glGetUniformBlockIndex(m_Shader->getProgramID(), "LightsUBO");
        glUniformBlockBinding(m_Shader->getProgramID(), lightsUBO_Index, 1);
    }

    // Re-bind for the light cube shader
    if (m_LightCubeShader && m_LightCubeShader->getProgramID() != 0)
    {
        unsigned int lightCube_UBO_Index = glGetUniformBlockIndex(m_LightCubeShader->getProgramID(), "CameraUBO");
        glUniformBlockBinding(m_LightCubeShader->getProgramID(), lightCube_UBO_Index, 0);
    }

    // Re-bind for the guide shader
    if (m_GuideShader && m_GuideShader->getProgramID() != 0)
    {
        unsigned int guide_UBO_Index = glGetUniformBlockIndex(m_GuideShader->getProgramID(), "CameraUBO");
        glUniformBlockBinding(m_GuideShader->getProgramID(), guide_UBO_Index, 0);
    }
}

void Chapter19_Application::setupSkybox()
{
    m_skyboxShader = std::make_unique<Base::Shader>();
    m_skyboxShader->loadFromFile("shaders/cubemap.vert", "shaders/cubemap.frag");
    m_skyboxShader->use();
    m_skyboxShader->setInt("cubemap", 0);

    std::vector<std::string> faces = {
        "images/Yokohama/posx.jpg",
        "images/Yokohama/negx.jpg",
        "images/Yokohama/posy.jpg",
        "images/Yokohama/negy.jpg",
        "images/Yokohama/posz.jpg",
        "images/Yokohama/negz.jpg"
    };
    m_skyboxTexture = std::make_unique<Base::Texture>();
    if (!m_skyboxTexture->loadCubemap(faces))
    {
        LOG_ERROR("Failed to load skybox textures.");
    }
    
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_skyboxVao);
    glGenBuffers(1, &m_skyboxVbo);
    glBindVertexArray(m_skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void Chapter19_Application::renderSkybox()
{
    glDepthFunc(GL_LEQUAL);

    m_skyboxShader->use();
    m_skyboxTexture->bind(0);
    
    glBindVertexArray(m_skyboxVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}