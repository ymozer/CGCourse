#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "Shader.hpp"
#include "Texture.hpp"

namespace Base
{
    struct ModelVertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct ModelTexture
    {
        std::shared_ptr<Base::Texture> texture;
        std::string type; // "texture_diffuse" or "texture_specular"
    };

    class Mesh
    {
    public:
        std::vector<ModelVertex>       vertices;
        std::vector<unsigned int>      indices;
        std::vector<ModelTexture>      textures;
        GLuint VAO;

        Mesh(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, std::vector<ModelTexture> textures);
        void Draw(Base::Shader& shader);

    private:
        GLuint VBO, EBO;
        void setupMesh();
    };


    class Model
    {
    public:
        std::vector<ModelTexture> textures_loaded;

        Model(const std::string& path);
        void Draw(Base::Shader& shader);

    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<ModelTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}