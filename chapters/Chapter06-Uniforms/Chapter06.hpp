#pragma once

#include "IChapter.hpp"
#include "Shader.hpp"

class Chapter06_Application : public IChapter
{
public:
    Chapter06_Application();

protected:
    void setup() override;
    void shutdown() override;
    void render() override;
    void renderChapterUI() override;
    void update(float deltaTime) override;
    void handleInput(float deltaTime) override;

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;

    // C++ variables to hold our colors
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_SquareColor[4] = {1.0f, 0.5f, 0.2f, 1.0f}; // Orange

    // A variable to store the integer location of our shader uniform
    GLint m_SquareColorLocation = -1;
};