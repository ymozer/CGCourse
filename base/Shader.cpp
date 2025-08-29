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

    GLint Shader::getUniformLocation(const std::string &name) const
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        {
            return m_UniformLocationCache[name];
        }

        GLint location = glGetUniformLocation(m_ID, name.c_str());
        if (location == -1)
        {
            // Use your logger to warn that a specific uniform was not found.
            // This is the key diagnostic message you need.
            LOG_WARN("Uniform '{}' not found in shader program!", name);
        }

        m_UniformLocationCache[name] = location;
        return location;
    }

    bool Shader::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
    {
        m_VertexPath = vertexPath;
        m_FragmentPath = fragmentPath;

        std::string fullVertexPath = resolveAssetPath(vertexPath);
        std::string fullFragmentPath = resolveAssetPath(fragmentPath);

        std::vector<char> vertexBuffer;
        std::vector<char> fragmentBuffer;

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

        std::string errorLog;
        if (compileFromSource(vertexBuffer.data(), fragmentBuffer.data(), errorLog))
        {
            updateFileTimestamps();
            return true;
        }

        return false;
    }

    bool Shader::compileFromSource(const char *vShaderCode, const char *fShaderCode, std::string &outErrorLog)
    {
        outErrorLog.clear();

        const char *definitions[] =
            {
                GLSL_VERSION_STRING "\n",
                GLSL_PRECISION_STRING "\n"};

        LOG_DEBUG("--- Compiling Vertex Shader Source ---\n{}{}{}", definitions[0], definitions[1], vShaderCode);
        LOG_DEBUG("--- Compiling Fragment Shader Source ---\n{}{}{}", definitions[0], definitions[1], fShaderCode);

        const GLubyte *version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        LOG_DEBUG("SHADING_LANGUAGE_VERSION: {}", reinterpret_cast<const char *>(version));

        GLuint vertex, fragment;

        const char *vertexSources[] = {definitions[0], vShaderCode}; // Vert shaders don't need precision
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 2, vertexSources, NULL);
        glCompileShader(vertex);
        if (!checkCompileErrors(vertex, "VERTEX", outErrorLog))
        {
            glDeleteShader(vertex);
            return false;
        }

        const char *fragmentSources[] = {definitions[0], definitions[1], fShaderCode};
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 3, fragmentSources, NULL);
        glCompileShader(fragment);
        if (!checkCompileErrors(fragment, "FRAGMENT", outErrorLog))
        {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        GLuint newProgramID = glCreateProgram();
        glAttachShader(newProgramID, vertex);
        glAttachShader(newProgramID, fragment);
        glLinkProgram(newProgramID);

        // Check for linking errors
        if (!checkCompileErrors(newProgramID, "PROGRAM", outErrorLog))
        {
            // If linking fails, clean up the new program and shaders and return false.
            // The old m_ID is left untouched.
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteProgram(newProgramID);
            return false;
        }

        // If linking succeeds, delete the OLD program (if it exists)
        if (m_ID != 0)
        {
            glDeleteProgram(m_ID);
        }

        // Assign the new, successfully linked program
        m_ID = newProgramID;
        m_UniformLocationCache.clear();

        // Clean up shaders as they are now linked into the program
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        LOG_INFO("Shader compiled and linked successfully. Program ID: {}", m_ID);
        return true;
    }

    void Shader::use() const
    {
        glUseProgram(m_ID);
    }

    void Shader::setBool(const std::string &name, bool value) const
    {
        glUniform1i(getUniformLocation(name), static_cast<int>(value));
    }
    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }
    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(getUniformLocation(name), value);
    }
    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }
    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    bool Shader::checkCompileErrors(GLuint shader, const std::string &type, std::string& outErrorLog)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                outErrorLog = "COMPILE ERROR (" + type + "):\n" + infoLog;
                return false;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                outErrorLog = std::string("LINKING ERROR:\n") + infoLog;
                return false;
            }
        }
        return true;
    }

    void Shader::updateFileTimestamps()
    {
        if (m_VertexPath.empty() || m_FragmentPath.empty()) return;
        try
        {
            m_VertexFileTime = std::filesystem::last_write_time(resolveAssetPath(m_VertexPath));
            m_FragmentFileTime = std::filesystem::last_write_time(resolveAssetPath(m_FragmentPath));
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            LOG_WARN("Could not update shader timestamp: {}", e.what());
        }
    }

    bool Shader::tryReload()
    {
        if (m_VertexPath.empty() || m_FragmentPath.empty())
        {
            return false;
        }

        try
        {
            std::string fullVertexPath = resolveAssetPath(m_VertexPath);
            std::string fullFragmentPath = resolveAssetPath(m_FragmentPath);

            auto currentVertexTime = std::filesystem::last_write_time(fullVertexPath);
            auto currentFragmentTime = std::filesystem::last_write_time(fullFragmentPath);

            if (currentVertexTime > m_VertexFileTime || currentFragmentTime > m_FragmentFileTime)
            {
                LOG_INFO("Shader change detected, attempting to reload: {}", m_VertexPath);

                bool success = loadFromFile(m_VertexPath, m_FragmentPath);

                m_VertexFileTime = currentVertexTime;
                m_FragmentFileTime = currentFragmentTime;

                if (success)
                {
                    LOG_INFO("Hot reload succeeded.");
                }
                else
                {
                    LOG_WARN("Hot reload failed. Reverting to the last working shader.");
                }

                return success;
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            LOG_WARN("Could not get file timestamp: {}", e.what());
        }

        return false;
    }

} // namespace Base