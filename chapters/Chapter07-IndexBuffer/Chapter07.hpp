#include "IChapter.hpp"
#include "Shader.hpp"

class Chapter07_Application : public IChapter
{
public:
    Chapter07_Application();

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
    GLuint m_EboID = 0;
    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_SquareColor[4] = {0.2f, 0.5f, 1.0f, 1.0f};
};