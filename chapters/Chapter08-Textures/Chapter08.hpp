#pragma once

#include "ChapterPreamble.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <memory>

class Chapter08_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
    Chapter08_Application(std::string title, int width, int height);
#else
    Chapter08_Application();
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
    std::unique_ptr<Base::Texture> m_Texture;
    GLuint m_VaoID = 0;
    GLuint m_VboID = 0;
    GLuint m_EboID = 0;
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_SquareTintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};