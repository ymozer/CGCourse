#include "ShaderEditor.hpp"
#include "Log.hpp"
#include <imgui.h>
#include <SDL3/SDL_iostream.h>
#include <fstream>

namespace Base {

ShaderEditor& ShaderEditor::getInstance()
{
    static ShaderEditor instance;
    return instance;
}

void ShaderEditor::registerShader(const std::string& name, Shader* shader)
{
    m_Shaders[name] = shader;
    if (m_SelectedShaderName.empty())
    {
        m_SelectedShaderName = name;
        loadSourceToBuffers();
    }
}

void ShaderEditor::render()
{
    if (!m_ShowPanel)
    {
        return;
    }

    ImGui::Begin("Shader Editor", &m_ShowPanel);

    // Dropdown to select shader
    if (ImGui::BeginCombo("Shader", m_SelectedShaderName.c_str()))
    {
        for (const auto& pair : m_Shaders)
        {
            bool is_selected = (m_SelectedShaderName == pair.first);
            if (ImGui::Selectable(pair.first.c_str(), is_selected))
            {
                if (m_SelectedShaderName != pair.first) {
                    m_SelectedShaderName = pair.first;
                    loadSourceToBuffers();
                }
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::Separator();

    if (!m_SelectedShaderName.empty())
    {
        // Radio buttons for Vertex/Fragment
        ImGui::RadioButton("Vertex", &m_SelectedShaderType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Fragment", &m_SelectedShaderType, 1);

        // Save & Compile button
        if (ImGui::Button("Save & Compile"))
        {
            saveAndRecompile();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(m_StatusColor.r, m_StatusColor.g, m_StatusColor.b, m_StatusColor.a), "%s", m_CompileStatusMessage.c_str());

        // Text editor widget
        ImGui::Separator();
        char* active_buffer = (m_SelectedShaderType == 0) ? m_VertexCodeBuffer.data() : m_FragmentCodeBuffer.data();
        ImGui::InputTextMultiline("##source", active_buffer, TEXT_BUFFER_SIZE, ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_AllowTabInput);
    }

    ImGui::End();
}

void ShaderEditor::loadSourceToBuffers()
{
    m_VertexCodeBuffer.assign(TEXT_BUFFER_SIZE, '\0');
    m_FragmentCodeBuffer.assign(TEXT_BUFFER_SIZE, '\0');
    m_CompileStatusMessage.clear();

    if (m_SelectedShaderName.empty() || m_Shaders.find(m_SelectedShaderName) == m_Shaders.end()) return;

    Shader* shader = m_Shaders[m_SelectedShaderName];
    
    auto readFile = [](const std::string& path, std::vector<char>& buffer) {
        std::ifstream file(Shader::resolveAssetPath(path), std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
             LOG_ERROR("Failed to open shader file for editor: {}", path);
             return;
        }
        size_t fileSize = (size_t) file.tellg();
        file.seekg(0);
        if(fileSize < buffer.size()) {
            file.read(buffer.data(), fileSize);
        }
    };

    readFile(shader->getVertexPath(), m_VertexCodeBuffer);
    readFile(shader->getFragmentPath(), m_FragmentCodeBuffer);
}

void ShaderEditor::saveAndRecompile()
{
    if (m_SelectedShaderName.empty() || m_Shaders.find(m_SelectedShaderName) == m_Shaders.end()) return;
    
    Shader* shader = m_Shaders[m_SelectedShaderName];
    const char* vertex_code = m_VertexCodeBuffer.data();
    const char* fragment_code = m_FragmentCodeBuffer.data();

    std::string errorLog;
    if (shader->compileFromSource(vertex_code, fragment_code, errorLog))
    {
        m_CompileStatusMessage = "Success!";
        m_StatusColor = { 0.0f, 1.0f, 0.0f, 1.0f };
        
        std::ofstream v_file(Shader::resolveAssetPath(shader->getVertexPath()));
        v_file << vertex_code;
        v_file.close();

        std::ofstream f_file(Shader::resolveAssetPath(shader->getFragmentPath()));
        f_file << fragment_code;
        f_file.close();

        shader->updateFileTimestamps(); 
    }
    else
    {
        m_CompileStatusMessage = errorLog;
        m_StatusColor = { 1.0f, 0.0f, 0.0f, 1.0f };
    }
}

} // namespace Base