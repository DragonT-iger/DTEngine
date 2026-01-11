#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "SimpleMathHelper.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BoneInfo
{
    int id;
    Matrix offset;
};

struct ModelImpl
{
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
};

Matrix ConvertToDXMatrix(const aiMatrix4x4& from)
{
    Matrix to;
    to._11 = from.a1; to._12 = from.b1; to._13 = from.c1; to._14 = from.d1;
    to._21 = from.a2; to._22 = from.b2; to._23 = from.c2; to._24 = from.d2;
    to._31 = from.a3; to._32 = from.b3; to._33 = from.c3; to._34 = from.d3;
    to._41 = from.a4; to._42 = from.b4; to._43 = from.c4; to._44 = from.d4;
    return to.Transpose();
}

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

        if (aiMesh->HasTangentsAndBitangents())
        {
            vertex.Tangent.x = aiMesh->mTangents[i].x;
            vertex.Tangent.y = aiMesh->mTangents[i].y;
            vertex.Tangent.z = aiMesh->mTangents[i].z;

            vertex.Bitangent.x = aiMesh->mBitangents[i].x;
            vertex.Bitangent.y = aiMesh->mBitangents[i].y;
            vertex.Bitangent.z = aiMesh->mBitangents[i].z;
        }
        else
        {
            vertex.Tangent = { 0.0f, 0.0f, 0.0f };
            vertex.Bitangent = { 0.0f, 0.0f, 0.0f };
        }
        for (int j = 0; j < 4; j++)
        {
            vertex.BoneIDs[j] = 0;
            vertex.Weights[j] = 0.0f;
        }
        vertices.push_back(vertex);
    }


    ExtractBoneWeightForVertices(vertices, aiMesh, scene);

    for (auto& v : vertices)
    {
        float totalWeight = v.Weights[0] + v.Weights[1] + v.Weights[2] + v.Weights[3];

        if (totalWeight <= 0.001f)
        {
            v.BoneIDs[0] = 0;      
            v.Weights[0] = 1.0f;   
            v.Weights[1] = 0.0f;
            v.Weights[2] = 0.0f;
            v.Weights[3] = 0.0f;
        }
        else
        {
            float scale = 1.0f / totalWeight;
            v.Weights[0] *= scale;
            v.Weights[1] *= scale;
            v.Weights[2] *= scale;
            v.Weights[3] *= scale;
        }
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

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_impl->m_BoneInfoMap.find(boneName) == m_impl->m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_impl->m_BoneCounter;

            m_impl->m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_impl->m_BoneCounter;
            m_impl->m_BoneCounter++;
        }
        else
        {
            boneID = m_impl->m_BoneInfoMap[boneName].id;
        }

        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < 4; ++i)
    {
        if (vertex.BoneIDs[i] < 0)
        {
            vertex.BoneIDs[i] = boneID;
            vertex.Weights[i] = weight;
            return;
        }
    }
}