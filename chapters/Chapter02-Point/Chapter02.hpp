#pragma once

#include "IChapter.hpp"
#include "Shader.hpp"

class Chapter02_Application : public IChapter
{
public:
    Chapter02_Application();

protected:
    void setup() override;
    void shutdown() override;
    void render() override;
    void renderChapterUI() override;
    void handleInput(float deltaTime) override;
    void update(float deltaTime) override;

private:
    std::unique_ptr<Base::Shader> m_Shader;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};
