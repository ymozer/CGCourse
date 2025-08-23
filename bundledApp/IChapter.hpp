// in IChapter.hpp
#pragma once

class IChapter
{
public:
    virtual ~IChapter() = default;

    virtual void setup() = 0;
    virtual void shutdown() = 0;

    virtual void update(float deltaTime) = 0;
    virtual void handleInput(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void renderChapterUI() = 0;
};