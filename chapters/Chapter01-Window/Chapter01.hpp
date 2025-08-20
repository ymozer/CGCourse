#pragma once

#include "ChapterPreamble.hpp"

class Chapter01_Application : public ChapterBase
{
public:
#ifdef BUILD_STANDALONE
    Chapter01_Application(std::string title, int width, int height);
#else
    Chapter01_Application();
#endif

protected:
    void setup() override;
    void shutdown() override;
    void update(float deltaTime) override;
    void handleInput(float deltaTime) override;
    void render() override;
    void renderChapterUI() override;

private:
    float m_ClearColor[4] = { 0.1f, 0.1f, 0.2f, 1.0f };
};