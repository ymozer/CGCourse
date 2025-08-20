#pragma once

#include "ChapterPreamble.hpp"
#include "Shader.hpp"

class Chapter03_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
    Chapter03_Application(std::string title, int width, int height);
#else
    Chapter03_Application();
#endif

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
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
};
