#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "Model.hpp"
#include "Log.hpp"
#include "PathUtils.hpp"

namespace Base
{
    Mesh::Mesh(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, std::vector<ModelTexture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void *)0);
        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void *)offsetof(ModelVertex, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void *)offsetof(ModelVertex, TexCoords));

        glBindVertexArray(0);
    }

    void Mesh::Draw(Base::Shader &shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            shader.setInt((name + number).c_str(), i);
            textures[i].texture->bind(i);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    Model::Model(const std::string &path)
    {
        loadModel(path);
    }

    void Model::Draw(Base::Shader &shader)
    {
        for (unsigned int i = 0; i < m_meshes.size(); i++)
            m_meshes[i].Draw(shader);
    }

    // aiProcess_Triangulate: ensures all faces are triangles.
    // aiProcess_GenSmoothNormals: generates smooth normals if the model doesn't have them.
    // aiProcess_FlipUVs: flips texture coordinates on the y-axis, often needed.
    // aiProcess_CalcTangentSpace: calculates tangents and bitangents for normal mapping.
    void Model::loadModel(const std::string &path)
    {
        std::vector<char> fileBuffer = readAssetToBuffer(path);
        if (fileBuffer.empty())
        {
            LOG_ERROR("Aborting model load because file buffer is empty for: {}", path);
            return;
        }

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFileFromMemory(
            fileBuffer.data(), // Pointer to the data in memory
            fileBuffer.size(), // The size of the data
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace,
            nullptr); // We can leave the file extension hint as null

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG_ERROR("ASSIMP::ERROR::{}", importer.GetErrorString());
            return;
        }

        m_directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        LOG_INFO("Model loaded successfully from memory: {}", path);
    }

    void Model::processNode(aiNode *node, const aiScene *scene)
    {
        // Process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, scene));
        }
        // Then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<ModelVertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<ModelTexture> textures;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            ModelVertex vertex;
            vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            if (mesh->HasNormals())
            {
                vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            }
            if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
                vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process material
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. Diffuse maps
        std::vector<ModelTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        std::vector<ModelTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<ModelTexture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<ModelTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string relativeTexturePath(str.C_Str());

            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].texture->getPath().c_str(), relativeTexturePath.c_str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                ModelTexture modelTex;
                auto texture = std::make_shared<Base::Texture>();
                std::string fullTexturePath = m_directory + '/' + relativeTexturePath;
                if (texture->loadFromFile(fullTexturePath)) // Use your existing Texture class!
                {
                    texture->setPath(relativeTexturePath);
                    modelTex.texture = texture;
                    modelTex.type = typeName;
                    textures.push_back(modelTex);
                    textures_loaded.push_back(modelTex);
                }
                else
                {
                    LOG_WARN("Failed to load texture: {}", fullTexturePath);
                }
            }
        }
        return textures;
    }
}