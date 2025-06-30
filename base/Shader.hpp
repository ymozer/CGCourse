
#pragma once
#include <string>
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
#define GLSL_VERSION_STRING "#version 410"
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS
#define GLSL_VERSION_STRING "#version 300 es"
#elif PLATFORM_ANDROID
#define GLSL_VERSION_STRING "#version 320 es"
#endif

namespace Base {

class Shader {
public:
    Shader() = default;
    ~Shader();

    inline static std::string resolveAssetPath(const std::string& relativePath) {
        #ifdef __ANDROID__
            return relativePath;
        #else
            return "assets/" + relativePath;
        #endif
    }

    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool compileFromSource(const char* vShaderCode, const char* fShaderCode);
    
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    GLuint getProgramID() const {return m_ID;} 

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
private:
    GLuint m_ID = 0;
    bool checkCompileErrors(GLuint shader, const std::string& type);
};

} // namespace Base