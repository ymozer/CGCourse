// in IChapter.hpp
#pragma once

// An interface that defines what every "Chapter" must be able to do.
class IChapter
{
public:
    virtual ~IChapter() = default;

    // Called once when the chapter is selected and becomes active.
    virtual void setup() = 0;
    // Called once when the chapter is deselected.
    virtual void shutdown() = 0;

    // Called every frame.
    virtual void update(float deltaTime) = 0;
    virtual void handleInput(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void renderChapterUI() = 0;
};