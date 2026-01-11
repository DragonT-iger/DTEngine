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

    

        if (aiMesh->HasTangentsAndBitangents() && aiMesh->HasNormals())
        {
            DirectX::XMFLOAT3 N = {
               aiMesh->mNormals[i].x,
               aiMesh->mNormals[i].y,
               aiMesh->mNormals[i].z
            };
            DirectX::XMFLOAT3 T = {
                aiMesh->mTangents[i].x,
                aiMesh->mTangents[i].y,
                aiMesh->mTangents[i].z
            };
            DirectX::XMFLOAT3 B = {
                aiMesh->mBitangents[i].x,
                aiMesh->mBitangents[i].y,
                aiMesh->mBitangents[i].z
            };

            auto norm3 = [](DirectX::XMFLOAT3 v) //Normalize
                {
                    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
                    if (len > 1e-8f) { v.x /= len; v.y /= len; v.z /= len; }
                    return v;
                };
            N = norm3(N); T = norm3(T); B = norm3(B);

            DirectX::XMFLOAT3 c = {
                N.y * T.z - N.z * T.y,
                N.z * T.x - N.x * T.z,
                N.x * T.y - N.y * T.x
            };
            float d = c.x * B.x + c.y * B.y + c.z * B.z;

            float w = (d < 0.0f) ? -1.0f : 1.0f;
            vertex.Tangent = { T.x, T.y, T.z, w };
        }
       
        else
        {
            vertex.Tangent = { 0.0f, 0.0f, 0.0f, 0.0f };
            
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