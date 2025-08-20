// In a new HostApplication.hpp
#include "Application.hpp"
#include "IChapter.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>

class BundledApp : public Base::Application
{
public:
    BundledApp() : Application("OpenGL Course Chapters", 1280, 720) {}

protected:
    void setup() override;
    void shutdown() override;

    // The host's update/render will delegate to the current chapter.
    void update(float deltaTime) override;
    void render() override;
    void renderChapterUI() override;
    void handleInput(float deltaTime) override;


private:
    // A function to switch which chapter is running.
    void switchChapter(int chapterIndex);

    // A pointer to the currently active chapter. unique_ptr handles memory for us.
    std::unique_ptr<IChapter> m_CurrentChapter;
    int m_CurrentChapterIndex = -1;

    // A list of all available chapters.
    // We store "factory functions" that know how to create each chapter.
    std::vector<std::pair<std::string, std::function<std::unique_ptr<IChapter>()>>> m_AvailableChapters;
};