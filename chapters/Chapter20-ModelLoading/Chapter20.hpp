#pragma once

#include "ChapterPreamble.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "MaterialPresets.hpp"
#include "EventBus.hpp"
#include "Model.hpp"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

#define MAX_LIGHTS 16


struct Light;


class Chapter20_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
    Chapter20_Application(std::string title, int width, int height);
#else
    Chapter20_Application();
#endif
    ~Chapter20_Application();
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
    std::unique_ptr<Base::Model> m_ourModel;

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
    std::vector<glm::vec3> m_CubePositions;
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    enum class LightType
    {
        Directional = 0,
        Point = 1,
        Spot = 2
    };
    std::unique_ptr<Light> m_pLight[MAX_LIGHTS];
    int m_ActiveLights = 4;
    GLuint m_LightsUboID;
    int m_EditedLightIndex = 0;

    // Face Culling Settings
    bool m_FaceCullingEnabled = true;
    int m_CullFaceMode = 0;     // 0 for GL_BACK, 1 for GL_FRONT
    int m_WindingOrderMode = 0; // 0 for GL_CCW, 1 for GL_CW

    // Material Properties
    std::vector<Material> m_MaterialPresets;
    int m_CurrentMaterialIndex = 0;

    // Skybox Resources
    std::unique_ptr<Base::Shader> m_skyboxShader;
    std::unique_ptr<Base::Texture> m_skyboxTexture;
    GLuint m_skyboxVao = 0;
    GLuint m_skyboxVbo = 0;
    bool m_showSkybox = true;

    void setupSkybox();
    void renderSkybox();

    bool m_showGrid = true;

    void setupScene();
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
    
    void rebindShaderUniformBlocks();
};
