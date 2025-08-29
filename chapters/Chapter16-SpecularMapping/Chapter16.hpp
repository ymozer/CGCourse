#pragma once

#include "ChapterPreamble.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "MaterialPresets.hpp" 
#include "EventBus.hpp"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

struct Light;

class Chapter16_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
Chapter16_Application(std::string title, int width, int height);
#else
Chapter16_Application();
#endif
~Chapter16_Application();
protected:
    void setup() override;
    void shutdown() override;
    void render() override;
    void renderChapterUI() override;
    void update(float deltaTime) override;
    void handleInput(float deltaTime) override;
#ifdef BUILD_STANDALONE
    Camera *getActiveCamera() override { return &m_Camera; }
#else
    Camera *getActiveCamera() { return &m_Camera; }
#endif

private:
    // Event Bus Subscriptions
    Base::SubscriptionHandle m_mouseButtonSub;
    Base::SubscriptionHandle m_keyPressSub;

    // Cube Objects
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_DiffuseTexture;
    std::unique_ptr<Base::Texture> m_SpecularTexture;
    GLuint m_VaoID = 0, m_VboID = 0, m_EboID = 0;
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_RotationEuler = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    float m_TintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    bool m_UseTexture = true;

    // Guide Objects
    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;

    // Light Cube Objects
    std::unique_ptr<Base::Shader> m_LightCubeShader;
    GLuint m_LightCubeVaoID = 0;

    // Camera Objects
    Camera m_Camera;
    GLuint m_CameraUboID = 0;

    // Scene Objects
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    std::unique_ptr<Light> m_pLight = std::make_unique<Light>();

    // Face Culling Settings
    bool m_FaceCullingEnabled = true;
    int m_CullFaceMode = 0;     // 0 for GL_BACK, 1 for GL_FRONT
    int m_WindingOrderMode = 0; // 0 for GL_CCW, 1 for GL_CW

    // Material Properties
    std::vector<Material> m_MaterialPresets;
    int m_CurrentMaterialIndex = 0;

    void setupShaders();
    void setupGeometry();
    void setupCamera();
    void setupEventListeners();
    void setupCube();
    void setupCoordinateGuide();
    void setupLightCube();
    void drawMouseCapturePopup();
    void drawSceneSettingsUI();
    void drawLightSettingsUI();
    void drawCubeTransformUI();
    void drawCameraSettingsUI();
    void drawCullingSettingsUI();

};
