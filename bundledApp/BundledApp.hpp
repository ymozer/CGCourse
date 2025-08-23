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

    void update(float deltaTime) override;
    void render() override;
    void renderChapterUI() override;
    void handleInput(float deltaTime) override;

private:
    void switchChapter(int chapterIndex);

    int m_CurrentChapterIndex = -1;
    std::unique_ptr<IChapter> m_CurrentChapter;

    std::vector<std::pair<std::string, std::function<std::unique_ptr<IChapter>()>>> m_AvailableChapters;
};