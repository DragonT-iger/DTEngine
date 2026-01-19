#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "SimpleMathHelper.h"
#include "SkeletalRsource.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




//Helper
bool HasBones(const aiScene* scene)
{
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        if (mesh->HasBones())
        {
            return true;
        }
    }
    return false;
}



Model::Model() {
    m_impl = std::make_unique<BoneResource>();
}

Model::~Model()
{
    Unload();
}

void Model::Unload()
{
    //for (Mesh* mesh : m_meshes)
    //{
    //    delete mesh;
    //}
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
    std::cout << fullPath << std::endl;

    bool isRigged = false;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        if (scene->mMeshes[i]->HasBones()) {
            isRigged = true;
            break;
        }
    }

    if (isRigged)
    {
        ProcessBonesMap(scene); //Mesh 순회하면서 Bone의 이름과 index를 저장해 놓음.  Mapping only ; 이름과 index offset matrix 
        constexpr int NodeIndex =-1;
        CreateSkeleton(scene->mRootNode, NodeIndex); //계층 정보 부모의 index; Default matrix를 저장.
    }

    ProcessNode(scene->mRootNode, scene);
    return true;
}

Mesh* Model::GetMesh(size_t index) const
{
    if (index >= m_meshes.size()) return nullptr;
    return m_meshes[index].get();
}

//여기서 누계시켜야 함. 
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

std::unique_ptr<Mesh> Model::ProcessMesh(aiMesh* aiMesh, const aiScene* scene)
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

    

        if (aiMesh->HasTangentsAndBitangents())//&& aiMesh->HasNormals())
        {
            vertex.Tangent.x = aiMesh->mTangents[i].x;
            vertex.Tangent.y = aiMesh->mTangents[i].y;
            vertex.Tangent.z = aiMesh->mTangents[i].z;
            vertex.Tangent.w = 1.0f;

            vertex.Bitangent.x = aiMesh->mBitangents[i].x;
            vertex.Bitangent.y = aiMesh->mBitangents[i].y;
            vertex.Bitangent.z = aiMesh->mBitangents[i].z;
        
        }
        else
        {
            vertex.Tangent = { 0.0f, 0.0f, 0.0f, 1.0f };
            vertex.Bitangent = { 0.0f, 0.0f, 0.0f };

        }
        for (int j = 0; j < 4; j++)
        {
            vertex.BoneIDs[j] = 0;
            vertex.Weights[j] = 0.0f;
        }
        vertices.push_back(vertex);
    }

    if (1) //Skeleton은 Mesh가 갖고 있음
    {

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
            else //zero devide 
            {
                 ///Weight Normalization 
                float scale = 1.0f / totalWeight;
                v.Weights[0] *= scale;
                v.Weights[1] *= scale;
                v.Weights[2] *= scale;
                v.Weights[3] *= scale;
            }
        }
    }


    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
    mesh->CreateBuffers(vertices, indices);
    return mesh;
}

std::map<std::string, Matrix> m_tempOffsetMap;



void Model::ProcessBonesMap(const aiScene* scene)
{
    // 메쉬에 영향을 주는 '진짜 뼈'들의 OffsetMatrix만 먼저 수집합니다.
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int b = 0; b < mesh->mNumBones; b++)
        {
            std::string boneName = mesh->mBones[b]->mName.C_Str();
            // 임시 맵에 OffsetMatrix 저장 (나중에 노드 생성 시 매칭)
            m_tempOffsetMap[boneName] = Matrix(&mesh->mBones[b]->mOffsetMatrix.a1).Transpose(); //coloum to row 
        }
    }
}

void Model::CreateSkeleton(const aiNode* node, int parentIndex)
{
    std::string nodeName = node->mName.C_Str();

    BoneNode newNode;
    newNode.name = nodeName;
    newNode.id = (int)m_impl->m_Bones.size();
    newNode.ParentIndex = parentIndex;

    newNode.DefaultLocalMatrix = Matrix(&node->mTransformation.a1).Transpose(); //coloum to row 

    if (parentIndex != -1) {
        newNode.GlobalMatrix = newNode.DefaultLocalMatrix * m_impl->m_Bones[parentIndex].GlobalMatrix;

    }
    else {
        newNode.GlobalMatrix = newNode.DefaultLocalMatrix;
    }

    if (m_tempOffsetMap.count(nodeName)) {
        newNode.OffsetMatrix = m_tempOffsetMap[nodeName];
    }
    else {
        newNode.OffsetMatrix = SimpleMathHelper::IdentityMatrix();;
    }

    m_impl->m_Bones.push_back(newNode);
    m_impl->m_BoneMapping[nodeName] = newNode.id;



    int myIndex = newNode.id;
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        CreateSkeleton(node->mChildren[i], myIndex);
    }
}


void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    int BoneCounter = 0;


    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_impl->m_BoneMapping.find(boneName) == m_impl->m_BoneMapping.end())
        {
            continue;
        }

        boneID = m_impl->m_BoneMapping[boneName];

        if (boneID == 7) std::cout << boneName << std::endl;

        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights; // 해당 뼈에 영향받는 Vertex 수 

        //해당 뼈에 영향을 받는 Vertex
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
        if (vertex.Weights[i] ==0) //취향차이
        {
            vertex.BoneIDs[i] = boneID;
            vertex.Weights[i] = weight;
            return;
        }
    }
}