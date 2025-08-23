#pragma once

#include "ChapterPreamble.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "EventBus.hpp"

#include <memory>

class Chapter11_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
    Chapter11_Application(std::string title, int width, int height);
#else
    Chapter11_Application();
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

    // Guide Objects
    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;

    // Camera Objects
    Camera m_Camera;

    // Scene Objects
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    void setupCube();
    void setupCoordinateGuide();
    void drawMouseCapturePopup();
};
