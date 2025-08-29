#pragma once
#include "Shader.hpp"
#include <string>
#include <map>
#include <vector>

namespace Base {

class ShaderEditor
{
public:
    static ShaderEditor& getInstance();

    void registerShader(const std::string& name, Shader* shader);
    void render();
    void show() { m_ShowPanel = true; }
    void hide() { m_ShowPanel = false; }
    void toggle() { m_ShowPanel = !m_ShowPanel; }

private:
    ShaderEditor() = default;
    ~ShaderEditor() = default;
    ShaderEditor(const ShaderEditor&) = delete;
    ShaderEditor& operator=(const ShaderEditor&) = delete;

    void loadSourceToBuffers();
    void saveAndRecompile();

    bool m_ShowPanel = false;

    std::map<std::string, Shader*> m_Shaders;
    std::string m_SelectedShaderName;
    int m_SelectedShaderType = 0; // 0 for Vertex, 1 for Fragment

    // Text Buffers for ImGui::InputTextMultiline
    static constexpr size_t TEXT_BUFFER_SIZE = 65536; // 64KB
    std::vector<char> m_VertexCodeBuffer;
    std::vector<char> m_FragmentCodeBuffer;
    
    std::string m_CompileStatusMessage;
    glm::vec4 m_StatusColor = { 0.8f, 0.8f, 0.8f, 1.0f };
};

} // namespace Base