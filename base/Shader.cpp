#include "Shader.hpp"
#include "Log.hpp"
#include <vector>
#include <SDL3/SDL_iostream.h>
#include <glm/gtc/type_ptr.hpp>

namespace Base
{

    Shader::~Shader()
    {
        if (m_ID != 0)
        {
            glDeleteProgram(m_ID);
        }
    }

    bool Shader::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
    {
        // --- 1. Retrieve the vertex/fragment source code using SDL_IOStream ---
        std::vector<char> vertexBuffer;
        std::vector<char> fragmentBuffer;

        // Helper function to read a file into a buffer using SDL_IOStream
        auto readFileSdl = [](const std::string &path, std::vector<char> &buffer) -> bool
        {
            LOG_DEBUG("Loading asset with SDL_IOStream: '{}'", path);

            SDL_IOStream *io = SDL_IOFromFile(path.c_str(), "rb");
            if (io == nullptr)
            {
                LOG_ERROR("SDL_IOFromFile failed for '{}': {}", path, SDL_GetError());
                return false;
            }

            Sint64 size = SDL_GetIOSize(io);
            if (size <= 0)
            {
                LOG_ERROR("SDL_GetIOSize failed or file is empty for '{}': {}", path, SDL_GetError());
                SDL_CloseIO(io);
                return false;
            }

            buffer.resize(static_cast<size_t>(size));
            size_t bytes_read = SDL_ReadIO(io, buffer.data(), static_cast<size_t>(size));
            SDL_CloseIO(io);

            if (bytes_read != static_cast<size_t>(size))
            {
                LOG_ERROR("SDL_ReadIO failed to read full file: {}", path);
                return false;
            }

            return true;
        };

        bool vLoaded = readFileSdl(resolveAssetPath(vertexPath), vertexBuffer);
        bool fLoaded = readFileSdl(resolveAssetPath(fragmentPath), fragmentBuffer);

        if (!vLoaded || !fLoaded)
        {
            LOG_ERROR("SHADER::LOAD_FAILED: Could not load one or both shader files via SDL_IOStream.");
            return false;
        }

        vertexBuffer.push_back('\0');
        fragmentBuffer.push_back('\0');

        const GLubyte *version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        LOG_DEBUG("SHADING_LANGUAGE_VERSION: {}", reinterpret_cast<const char *>(version));

        std::string vertexSource = vertexBuffer.data();
        std::string fragmentSource = fragmentBuffer.data();

        const char *vShaderCode = vertexSource.c_str();
        const char *fShaderCode = fragmentSource.c_str();

        LOG_DEBUG("--- Compiling Vertex Shader Source ---\n{}", vShaderCode);
        LOG_DEBUG("--- Compiling Fragment Shader Source ---\n{}", fShaderCode);

        // --- 2. Compile shaders ---
        GLuint vertex, fragment;

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        if (!checkCompileErrors(vertex, "VERTEX"))
        {
            glDeleteShader(vertex);
            return false;
        }

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        if (!checkCompileErrors(fragment, "FRAGMENT"))
        {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        // Shader Program
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        if (!checkCompileErrors(m_ID, "PROGRAM"))
        {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            m_ID = 0;
            return false;
        }

        // Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        LOG_INFO("Shader loaded successfully: {} & {}", vertexPath, fragmentPath);
        return true;
    }

    bool Shader::compileFromSource(const char *vShaderCode, const char *fShaderCode)
    {
        LOG_DEBUG("--- Compiling Vertex Shader Source ---\n{}", vShaderCode);
        LOG_DEBUG("--- Compiling Fragment Shader Source ---\n{}", fShaderCode);

        const GLubyte *version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        LOG_DEBUG("SHADING_LANGUAGE_VERSION: {}", reinterpret_cast<const char *>(version));

        GLuint vertex, fragment;

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        if (!checkCompileErrors(vertex, "VERTEX"))
        {
            glDeleteShader(vertex);
            return false;
        }

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        if (!checkCompileErrors(fragment, "FRAGMENT"))
        {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        // Shader Program
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        if (!checkCompileErrors(m_ID, "PROGRAM"))
        {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            m_ID = 0;
            return false;
        }

        // Clean up shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        LOG_INFO("Shader compiled successfully from source.");
        return true;
    }

    void Shader::use() const
    {
        glUseProgram(m_ID);
    }

    // Utility uniform functions
    void Shader::setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
    }
    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }
    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }
    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
    {
        GLint location = glGetUniformLocation(m_ID, name.c_str());
        if (location == -1)
        {
            LOG_WARN("Uniform '{}' not found or inactive in shader.", name);
            return;
        }
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    bool Shader::checkCompileErrors(GLuint shader, const std::string &type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                LOG_ERROR("SHADER_COMPILATION_ERROR of type: {}\n{}\n -- --------------------------------------------------- -- ", type, infoLog);
                return false;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                LOG_ERROR("PROGRAM_LINKING_ERROR of type: {}\n{}\n -- --------------------------------------------------- -- ", type, infoLog);
                return false;
            }
        }
        return true;
    }

} // namespace Base