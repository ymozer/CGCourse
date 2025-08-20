#include "Application.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <memory>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>

class Chapter04_Application : public IChapter
{
public:
    Chapter04_Application() : Application("Chapter 4: Cube Transformations") {}

protected:
    void setup() override
    {
        m_Shader = std::make_unique<Base::Shader>();
        m_Shader->loadFromFile("shaders/chapter04.vert", "shaders/chapter04.frag");

        // === 1. DEFINE VERTEX DATA FOR A CUBE ===
        // We need 24 vertices (4 for each of the 6 faces) to have correct
        // unique texture coordinates for each face.
        float vertices[] = {
            // positions          // texture Coords
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            // Back face
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            // Left face
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            // Right face
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            // Bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            // Top face
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f
        };

        // === 2. DEFINE INDEX DATA FOR THE CUBE ===
        // Each face is a quad, made of 2 triangles (6 indices).
        // 6 faces * 6 indices = 36 indices total.
        unsigned int indices[] = {
            0, 1, 2,   2, 3, 0,     // Front
            4, 5, 6,   6, 7, 4,     // Back
            8, 9, 10,  10, 11, 8,   // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Bottom
            20, 21, 22, 22, 23, 20  // Top
        };

        glGenVertexArrays(1, &m_VaoID);
        glGenBuffers(1, &m_VboID);
        glGenBuffers(1, &m_EboID);
        
        glBindVertexArray(m_VaoID);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Attribute pointers are unchanged, as the vertex format is the same.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_Texture = std::make_unique<Base::Texture>();
        m_Texture->loadFromFile("assets/images/uv.png");
        
        // Setup for coordinate guide (unchanged)
        setupCoordinateGuide();
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &m_VaoID);
        glDeleteBuffers(1, &m_VboID);
        glDeleteBuffers(1, &m_EboID);
        m_Shader.reset();
        m_Texture.reset();

        glDeleteVertexArrays(1, &m_GuideVaoID);
        glDeleteBuffers(1, &m_GuideVboID);
        m_GuideShader.reset();
    }

    void update(float deltaTime) override
    {
        // Model matrix calculation is unchanged. It works for any mesh.
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
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), getViewportAspectRatio(), 0.1f, 100.0f);

        // --- Draw the Cube ---
        m_Shader->use();
        glm::mat4 transform = projection * view * m_ModelMatrix;
        m_Shader->setMat4("u_Transform", transform);
        m_Shader->setInt("u_Texture", 0);
        m_Shader->setVec4("u_TintColor", glm::make_vec4(m_TintColor));
        m_Texture->bind(0);
        glBindVertexArray(m_VaoID);
        // === 3. UPDATE DRAW CALL TO DRAW 36 INDICES ===
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Draw the coordinate guide (unchanged)
        if (m_ShowCoordinateGuide) {
            drawCoordinateGuide(projection * view);
        }
        
        glBindVertexArray(0);
    }

    void renderChapterUI() override
    {
        ImGui::Begin("Cube Transformations");
        
        ImGui::Text("Translate");
        ImGui::DragFloat3("Position", &m_Position.x, 0.01f);

        ImGui::Text("Rotate (Euler Angles)");
        ImGui::DragFloat3("Rotation", &m_RotationEuler.x, 1.0f, -180.0f, 180.0f);

        ImGui::Text("Scale");
        ImGui::DragFloat3("Size", &m_Scale.x, 0.01f);
        
        ImGui::Separator();
        if (ImGui::Button("Reset")) {
            m_Position = glm::vec3(0.0f);
            m_RotationEuler = glm::vec3(0.0f);
            m_Scale = glm::vec3(1.0f);
        }

        ImGui::Separator();
        ImGui::Checkbox("Show Coordinate Guide", &m_ShowCoordinateGuide);
        ImGui::Separator();

        ImGui::Text("Appearance");
        ImGui::ColorEdit3("Background Color", m_ClearColor);
        ImGui::ColorEdit4("Cube Tint Color", m_TintColor);
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("Model Matrix (Read-Only)", ImGuiTreeNodeFlags_DefaultOpen)) 
        {
            ImGui::Text("This matrix represents the cube's transformation in the world space.");
            ImGui::Text("It is calculated from the position, rotation, and scale values you set.");
            ImGui::Text("You can copy this matrix to use in other applications or shaders.");
            // ImGui stores matrices in column-major order, so we can pass the pointer directly.
            const float* matrix = glm::value_ptr(m_ModelMatrix);
            ImGui::InputFloat4("##Row1", (float*)&matrix[0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##Row2", (float*)&matrix[4], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##Row3", (float*)&matrix[8], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##Row4", (float*)&matrix[12], "%.2f", ImGuiInputTextFlags_ReadOnly);
        }
        
        ImGui::End();
    }

    void handleInput(float deltaTime) override
    {
        // Handle input events here if needed.
    }

private:
    void setupCoordinateGuide() {
        m_GuideShader = std::make_unique<Base::Shader>();
        m_GuideShader->loadFromFile("shaders/guide.vert", "shaders/guide.frag");
        float guideVertices[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
        glGenVertexArrays(1, &m_GuideVaoID);
        glGenBuffers(1, &m_GuideVboID);
        glBindVertexArray(m_GuideVaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_GuideVboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(guideVertices), guideVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void drawCoordinateGuide(const glm::mat4& pvMatrix) {
        m_GuideShader->use();
        m_GuideShader->setMat4("u_Transform", pvMatrix); // Uses projection * view
        glLineWidth(2.5f);
        glBindVertexArray(m_GuideVaoID);
        glDrawArrays(GL_LINES, 0, 6);
        glLineWidth(1.0f);
    }

    // Objects for the cube
    std::unique_ptr<Base::Shader> m_Shader;
    std::unique_ptr<Base::Texture> m_Texture;
    GLuint m_VaoID = 0, m_VboID = 0, m_EboID = 0;

    // Objects for the coordinate guide
    std::unique_ptr<Base::Shader> m_GuideShader;
    GLuint m_GuideVaoID = 0, m_GuideVboID = 0;
    bool m_ShowCoordinateGuide = true;
    
    // UI-controlled transformation values
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_RotationEuler = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);

    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

    float m_ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    float m_TintColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};

int main(int argc, char* argv[])
{
    try {
        auto app = std::make_unique<Chapter04_Application>();
        app->run();
    } catch (const std::exception& e) {
        LOG_ERROR("Fatal error: {}", e.what());
        return -1;
    }
    return 0;
}