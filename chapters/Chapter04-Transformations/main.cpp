#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <memory>
#include <imgui.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

class Chapter04_Application : public Base::Application
{
public:
    Chapter04_Application() : Application("Chapter 4: Transformations") {}

protected:
    void setup() override
    {
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile("shaders/chapter04.vert", "shaders/chapter04.frag");

        // The vertex and index data for an efficient square is perfect for this.
        float vertices[] = {
            // positions         // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right (0)
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right (1)
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left (2)
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left (3)
        };
        unsigned int indices[] = {0, 1, 3, 1, 2, 3};

        glGenVertexArrays(1, &m_VaoID);
        glGenBuffers(1, &m_VboID);
        glGenBuffers(1, &m_EboID);

        glBindVertexArray(m_VaoID);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_Texture = std::make_unique<Base::Texture>();
        if (!m_Texture->loadFromFile("assets/images/uv.png"))
        {
            LOG_ERROR("Failed to load texture!");
        }

        {
            m_GuideShader = std::make_unique<Base::Shader>();
            m_GuideShader->loadFromFile("shaders/guide.vert", "shaders/guide.frag");

            // Vertex data for 3 lines: X (Red), Y (Green), Z (Blue)
            float guideVertices[] = {
                // positions         // colors
                // X-Axis
                0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // origin, red
                1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // end, red
                // Y-Axis
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // origin, green
                0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // end, green
                // Z-Axis
                0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // origin, blue
                0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // end, blue
            };

            glGenVertexArrays(1, &m_GuideVaoID);
            glGenBuffers(1, &m_GuideVboID);

            glBindVertexArray(m_GuideVaoID);

            glBindBuffer(GL_ARRAY_BUFFER, m_GuideVboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(guideVertices), guideVertices, GL_STATIC_DRAW);

            // Attribute pointers for the guide's VBO (position and color)
            const GLsizei guideStride = 6 * sizeof(float);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, guideStride, (void *)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, guideStride, (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &m_VaoID);
        glDeleteBuffers(1, &m_VboID);
        glDeleteBuffers(1, &m_EboID);
        glDeleteVertexArrays(1, &m_GuideVaoID);
        glDeleteBuffers(1, &m_GuideVboID);
        m_GuideShader.reset();
        m_Shader.reset();
        m_Texture.reset();
    }

    void update(float deltaTime) override
    {
        // The order of transformations is important: Scale -> Rotate -> Translate.
        // This is because matrix multiplication is read from right to left.
        // final_pos = Translation * Rotation * Scale * original_pos;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        model = glm::rotate(model, glm::radians(m_RotationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_RotationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_RotationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, m_Scale);

        m_ModelMatrix = model;
    }

    void render() override
    {
        glEnable(GL_DEPTH_TEST);

        // For 2D transformations, we don't need depth testing.
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->use();

        // Orthographic projection is suitable for 2D rendering.
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

        // The final transformation is Projection * View * Model.
        glm::mat4 transform = projection * view * m_ModelMatrix;
        m_Shader->setMat4("u_Transform", transform);
        m_Shader->setInt("u_Texture", 0);
        m_Shader->setVec4("u_TintColor", glm::make_vec4(m_TintColor));
        m_Texture->bind(0);

        glBindVertexArray(m_VaoID);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (m_ShowCoordinateGuide)
        {
            m_GuideShader->use();
            
            // The guide represents the world origin, so its model matrix is identity.
            glm::mat4 guideModel = glm::mat4(1.0f);
            glm::mat4 guideTransform = projection * view * guideModel;
            m_GuideShader->setMat4("u_Transform", guideTransform);

            // Make the lines thicker so they are easier to see
            glLineWidth(2.5f);
            
            glBindVertexArray(m_GuideVaoID);
            glDrawArrays(GL_LINES, 0, 6);
            
            // It's good practice to reset the line width
            glLineWidth(1.0f);
        }
        glBindVertexArray(0);
    }

    void renderChapterUI() override
    {
        // === NEW: Add a checkbox to toggle the guide ===

        ImGui::Begin("Transformations");
        
        ImGui::Checkbox("Show Coordinate Guide", &m_ShowCoordinateGuide);

        ImGui::Separator();

        ImGui::Text("Translate");
        ImGui::DragFloat3("Position", &m_Position.x, 0.01f);

        ImGui::Text("Rotate (Euler Angles)");
        ImGui::DragFloat3("Rotation", &m_RotationEuler.x, 1.0f, -180.0f, 180.0f);

        ImGui::Text("Scale");
        ImGui::DragFloat3("Size", &m_Scale.x, 0.01f);

        ImGui::Separator();
        if (ImGui::Button("Reset"))
        {
            m_Position = glm::vec3(0.0f);
            m_RotationEuler = glm::vec3(0.0f);
            m_Scale = glm::vec3(1.0f);
        }

        ImGui::Separator();
        ImGui::Text("Appearance");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Tint Color", m_TintColor);

        ImGui::End();
    }

    void handleInput(float deltaTime) override
    {
        // Handle input events here if needed.
        // For this chapter, we don't have specific input handling.
    }

private:
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;

    GLuint m_VaoID = 0, m_VboID = 0, m_EboID = 0;

    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;

    // UI-controlled transformation values
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_RotationEuler = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);

    // Matrix to store the result of the update() calculation
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_TintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};

int main(int argc, char *argv[])
{
    try
    {
        auto app = std::make_unique<Chapter04_Application>();
        app->run();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Fatal error: {}", e.what());
        return -1;
    }
    return 0;
}