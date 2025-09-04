
#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>

#include <glm/glm.hpp>
#if PLATFORM_DESKTOP
    #include <glad/gl.h>
#elif PLATFORM_ANDROID
    #include <glad/egl.h>
    #include <glad/gles2.h>
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS
    #include <glad/gles2.h>
#endif

#if PLATFORM_DESKTOP
#define GLSL_VERSION_STRING "#version 410 core"
#define GLSL_PRECISION_STRING ""
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS || PLATFORM_ANDROID
#define GLSL_VERSION_STRING "#version 300 es"
#define GLSL_PRECISION_STRING "precision mediump float;"
#endif

namespace Base {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool compileFromSource(const char *vShaderCode, const char *fShaderCode, std::string &outErrorLog);
    bool tryReload();
    void use() const;

    void updateFileTimestamps();
    void updateFileTimestamps(std::string vertexPath, std::string fragmentPath);
    GLuint getProgramID() const {return m_ID;} 
    const std::string& getVertexPath() const { return m_VertexPath; }
    const std::string& getFragmentPath() const { return m_FragmentPath; }

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    bool checkCompileErrors(GLuint shader, const std::string& type, std::string& outErrorLog);
    GLint getUniformLocation(const std::string &name) const;

private:
    GLuint m_ID = 0;
    mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
    std::string m_VertexPath;
    std::string m_FragmentPath;
    std::filesystem::file_time_type m_VertexFileTime;
    std::filesystem::file_time_type m_FragmentFileTime;
};

} // namespace Base