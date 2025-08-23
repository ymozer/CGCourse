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

struct Light
{
    glm::vec3 Position;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
};

class Chapter15_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
Chapter15_Application(std::string title, int width, int height);
#else
Chapter15_Application();
#endif
protected:
    void setup() override;
    void shutdown() override;
    void render() override;
    void renderChapterUI() override;
    void update(float deltaTime) override;
    void handleInput(float deltaTime) override;
    Camera *getActiveCamera() { return &m_Camera; }

private:
    // Event Bus Subscriptions
    Base::SubscriptionHandle m_mouseButtonSub;
    Base::SubscriptionHandle m_keyPressSub;

    // Cube Objects
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;
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

    // Scene Objects
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    Light m_Light = {
        /* Position */ {1.2f, 1.0f, 2.0f},
        /* Ambient  */ {0.2f, 0.2f, 0.2f},
        /* Diffuse  */ {1.0f, 1.0f, 1.0f},
        /* Specular */ {1.0f, 1.0f, 1.0f}
    };

    // Face Culling Settings
    bool m_FaceCullingEnabled = true;
    int m_CullFaceMode = 0;     // 0 for GL_BACK, 1 for GL_FRONT
    int m_WindingOrderMode = 0; // 0 for GL_CCW, 1 for GL_CW

    // Material Properties
    std::vector<Material> m_MaterialPresets;
    int m_CurrentMaterialIndex = 0;

    void setupCube();
    void setupCoordinateGuide();
    void setupLightCube();
    void drawMouseCapturePopup();
};
