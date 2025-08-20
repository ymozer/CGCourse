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

class Texture {
public:
    Texture();
    ~Texture();

    inline static std::string resolveAssetPath(const std::string& relativePath) {
        #ifdef __ANDROID__
            // On Android, paths are relative to the APK's assets root
            return relativePath;
        #else
            // On desktop, prepend the "assets/" directory
            return "assets/" + relativePath;
        #endif
    }

    bool loadFromFile(const std::string& path);

    void bind(GLuint textureUnit = 0) const;
    void unbind(GLuint textureUnit = 0) const;

    GLuint getID() const { return m_ID; }
    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }

    // Non-copyable
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

private:
    GLuint m_ID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_NrChannels = 0;
};

} // namespace Base