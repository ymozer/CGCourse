#define STBI_NO_JPEG
//#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Log.hpp"
#include "Texture.hpp"

namespace Base
{

    Texture::Texture()
    {
        glGenTextures(1, &m_ID);
    }

    Texture::~Texture()
    {
        if (m_ID != 0)
        {
            glDeleteTextures(1, &m_ID);
        }
    }

    bool Texture::loadFromFile(const std::string &path)
    {
        // OpenGL expects textures to be flipped vertically
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_NrChannels, 0);
        if (data)
        {
            GLenum format;
            if (m_NrChannels == 1)
                format = GL_RED;
            else if (m_NrChannels == 3)
                format = GL_RGB;
            else if (m_NrChannels == 4)
                format = GL_RGBA;
            else
            {
                LOG_ERROR("TEXTURE::LOAD_FAILED: Unsupported number of channels ({}) in image '{}'", m_NrChannels, path);
                stbi_image_free(data);
                return false;
            }

            glBindTexture(GL_TEXTURE_2D, m_ID);

            glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);

            LOG_INFO("Texture loaded successfully: {}", path);
            return true;
        }
        else
        {
            LOG_ERROR("TEXTURE::LOAD_FAILED: Failed to load image at path: {}", path);
            return false;
        }
    }

    void Texture::bind(GLuint textureUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }

    void Texture::unbind(GLuint textureUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // namespace Base