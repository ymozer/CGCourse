#pragma once

#include "IChapter.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

class Chapter09_Application : public IChapter
{
public:
    Chapter09_Application();

protected:
    void setup() override;
    void shutdown() override;
    void render() override;
    void renderChapterUI() override;
    void update(float deltaTime) override;
    void handleInput(float deltaTime) override;

private:
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;

    GLuint m_VaoID = 0, m_VboID = 0, m_EboID = 0;

    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;

    // UI-controlled transformation values
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_RotationEuler = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);

    // Matrix to store the result of the update() calculation
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_TintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};