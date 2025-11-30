#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model() {}

Model::~Model()
{
    Unload();
}

void Model::Unload()
{
    for (Mesh* mesh : m_meshes)
    {
        delete mesh;
    }
    m_meshes.clear();
}

bool Model::LoadFile(const std::string& fullPath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fullPath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return false;

    ProcessNode(scene->mRootNode, scene);
    return true;
}

Mesh* Model::GetMesh(size_t index) const
{
    if (index >= m_meshes.size()) return nullptr;
    return m_meshes[index];
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(ProcessMesh(aiMesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh* Model::ProcessMesh(aiMesh* aiMesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Pos.x = aiMesh->mVertices[i].x;
        vertex.Pos.y = aiMesh->mVertices[i].y;
        vertex.Pos.z = aiMesh->mVertices[i].z;

        if (aiMesh->HasNormals())
        {
            vertex.Normal.x = aiMesh->mNormals[i].x;
            vertex.Normal.y = aiMesh->mNormals[i].y;
            vertex.Normal.z = aiMesh->mNormals[i].z;
        }

        if (aiMesh->mTextureCoords[0])
        {
            vertex.Texcoord.x = aiMesh->mTextureCoords[0][i].x;
            vertex.Texcoord.y = aiMesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.Texcoord = { 0.0f, 0.0f };
        }

        if (aiMesh->HasVertexColors(0))
        {
            vertex.Color.x = aiMesh->mColors[0][i].r;
            vertex.Color.y = aiMesh->mColors[0][i].g;
            vertex.Color.z = aiMesh->mColors[0][i].b;
            vertex.Color.w = aiMesh->mColors[0][i].a;
        }
        else
        {
            vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        }
        
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Mesh* mesh = new Mesh();
    mesh->CreateBuffers(vertices, indices);
    return mesh;
}