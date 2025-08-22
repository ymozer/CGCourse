#pragma once

#include "ChapterPreamble.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <memory>

class Chapter10_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
    Chapter10_Application(std::string title, int width, int height);
#else
    Chapter10_Application();
#endif

protected:
    void setup() override;
    void shutdown() override;
    void render() override;
    void renderChapterUI() override;
    void update(float deltaTime) override;
    void handleInput(float deltaTime) override;

private:
    void setupCoordinateGuide() {
        m_GuideShader = std::make_unique<Base::Shader>();
        m_GuideShader->loadFromFile("shaders/guide.vert", "shaders/guide.frag");
        float guideVertices[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
        glGenVertexArrays(1, &m_GuideVaoID);
        glGenBuffers(1, &m_GuideVboID);
        glBindVertexArray(m_GuideVaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_GuideVboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(guideVertices), guideVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void drawCoordinateGuide(const glm::mat4& pvMatrix) {
        m_GuideShader->use();
        m_GuideShader->setMat4("u_Transform", pvMatrix); // Uses projection * view
        glLineWidth(2.5f);
        glBindVertexArray(m_GuideVaoID);
        glDrawArrays(GL_LINES, 0, 6);
        glLineWidth(1.0f);
    }

    // Objects for the cube
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;
    GLuint m_VaoID = 0, m_VboID = 0, m_EboID = 0;

    // Objects for the coordinate guide
    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;
    
    // UI-controlled transformation values
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_RotationEuler = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);

    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_TintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};
