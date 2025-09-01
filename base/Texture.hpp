#pragma once
#include <string>

#if PLATFORM_DESKTOP
    #include <glad/gl.h>
#elif PLATFORM_ANDROID
    #include <glad/egl.h>
    #include <glad/gles2.h>
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS
    #include <glad/gles2.h>
#endif

namespace Base {

enum class TextureType
{
    TEXTURE_2D,
    CUBEMAP
};

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path);
    bool loadCubemap(const std::vector<std::string>& faces);

    void bind(GLuint textureUnit = 0) const;
    void unbind(GLuint textureUnit = 0) const;

    GLuint getID() const { return m_ID; }
    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }
    std::string getPath() const { return m_Path; }
    void setPath(const std::string& path) { m_Path = path; }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

private:
    TextureType m_Type = TextureType::TEXTURE_2D;
    std::string m_Path;

    GLuint m_ID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_NrChannels = 0;
};

} // namespace Base