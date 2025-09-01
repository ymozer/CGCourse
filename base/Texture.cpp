//#define STBI_NO_JPEG
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
#include "PathUtils.hpp"

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
        m_Type = TextureType::TEXTURE_2D;
        // OpenGL expects textures to be flipped vertically
        stbi_set_flip_vertically_on_load(true);

        std::string fullPath = resolveAssetPath(path);
        size_t fileSize = 0;
        void* fileData = SDL_LoadFile(fullPath.c_str(), &fileSize);

         if (!fileData) {
            LOG_ERROR("TEXTURE::LOAD_FAILED: SDL_LoadFile could not open '{}': {}", fullPath, SDL_GetError());
            return false;
        }

        unsigned char *data = stbi_load_from_memory(
            static_cast<const stbi_uc*>(fileData),
            static_cast<int>(fileSize),
            &m_Width, &m_Height, &m_NrChannels, 0
        );

        SDL_free(fileData);

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
            m_Path = path;

            LOG_INFO("Texture loaded successfully: {}", path);
            return true;
        }
        else
        {
            LOG_ERROR("TEXTURE::LOAD_FAILED: Failed to load image at path: {}", path);
            return false;
        }
    }

    bool Texture::loadCubemap(const std::vector<std::string>& faces)
    {
        /*
            Face order: +X, -X, +Y, -Y, +Z, -Z
        */
        m_Type = TextureType::CUBEMAP;
        stbi_set_flip_vertically_on_load(false); // Cubemaps are typically not flipped

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

        for (unsigned int i = 0; i < faces.size(); i++)
        {
            LOG_TRACE("Loading cubemap face: {}", faces[i]);
            std::string fullPath = resolveAssetPath(faces[i]);
            size_t fileSize = 0;
            void* fileData = SDL_LoadFile(fullPath.c_str(), &fileSize);
            if (!fileData) {
                LOG_ERROR("CUBEMAP::LOAD_FAILED: Could not load face '{}': {}", fullPath, SDL_GetError());
                stbi_set_flip_vertically_on_load(true);
                return false;
            }

            unsigned char* data = stbi_load_from_memory(static_cast<const stbi_uc*>(fileData), static_cast<int>(fileSize), &m_Width, &m_Height, &m_NrChannels, 0);
            SDL_free(fileData);

            if (data)
            {
                 GLenum format;
                 if (m_NrChannels == 3) format = GL_RGB;
                 else if (m_NrChannels == 4) format = GL_RGBA;
                 else {
                     LOG_ERROR("CUBEMAP::LOAD_FAILED: Unsupported channel count {} for face '{}'", m_NrChannels, faces[i]);
                     stbi_image_free(data);
                     continue; // Try next face
                 }
                // Note: GL_TEXTURE_CUBE_MAP_POSITIVE_X is an enum that increments for each face
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                LOG_ERROR("CUBEMAP::LOAD_FAILED: stbi_load failed for face: {}", faces[i]);
                stbi_set_flip_vertically_on_load(true);
                return false;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
        if (!faces.empty()) {
            m_Path = faces[0];
        }

        LOG_INFO("Cubemap loaded successfully.");
        stbi_set_flip_vertically_on_load(true);
        return true;
    }


    void Texture::bind(GLuint textureUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(m_Type == TextureType::CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, m_ID);
    }

    void Texture::unbind(GLuint textureUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(m_Type == TextureType::CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, 0);
    }

} // namespace Base