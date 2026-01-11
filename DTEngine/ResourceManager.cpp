#include "pch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>


#include "ResourceManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SerializationUtils.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"




using namespace DirectX::SimpleMath;
namespace fs = std::filesystem;


static Matrix ConvertMatrix(const aiMatrix4x4& from)
{
    Matrix to;
    to._11 = from.a1; to._12 = from.a2; to._13 = from.a3; to._14 = from.a4;
    to._21 = from.b1; to._22 = from.b2; to._23 = from.b3; to._24 = from.b4;
    to._31 = from.c1; to._32 = from.c2; to._33 = from.c3; to._34 = from.c4;
    to._41 = from.d1; to._42 = from.d2; to._43 = from.d3; to._44 = from.d4;
    return to;
}

bool ResourceManager::Initialize(const std::string& resourceRootPath)
{
	m_resourceRootPath = resourceRootPath;
	return true;
}

void ResourceManager::Unload(const std::string& id)
{
	if (auto it = m_cache.find(id); it != m_cache.end()) {
		if (it->second) it->second->Unload();
		m_cache.erase(it);
	}
}

void ResourceManager::UnloadAll()
{
	for (auto& kv : m_cache)
		if (kv.second) kv.second->Unload();
	m_cache.clear();
}

std::string ResourceManager::ResolveFullPath(const std::string& path) const
{
    if (m_resourceRootPath.empty()) return path;

    fs::path pRoot(m_resourceRootPath); 
    fs::path pId(path);

    auto itRoot = pRoot.begin();
    auto itId = pId.begin();

    if (itRoot != pRoot.end() && itId != pId.end())
    {
        if (*itRoot == *itId)
        {
            return path;
			// Assets/Assets 경로 방지
        }
    }

    return (pRoot / pId).string();
}

void ResourceManager::CollectDescendants(GameObject* target, std::vector<GameObject*>& outList)
{
    outList.push_back(target);
    Transform* tf = target->GetTransform();
    if (tf)
    {
        for (Transform* child : tf->GetChildren())
        {
            CollectDescendants(child->_GetOwner(), outList);
        }
    }
}

bool ResourceManager::SavePrefab(GameObject* root, const std::string& fullPath)
{
    if (!root) return false;

    JsonWriter writer;
    std::string assetPath = ResolveFullPath(fullPath);

    std::vector<GameObject*> targets;
    CollectDescendants(root, targets);

    std::unordered_map<uint64_t, bool> prefabIDs;
    for (auto* go : targets)
    {
        prefabIDs[go->_GetID()] = true;
    }

    writer.BeginArray("gameObjects");

    for (auto& go : targets)
    {
        writer.NextArrayItem();

        writer.Write("id", go->_GetID());
        writer.Write("name", go->GetName());
        writer.Write("tag", go->GetTag());
        writer.Write("active", go->IsActive());

        uint64_t parentID = 0;
        Transform* parentTF = go->GetTransform()->GetParent();
        if (parentTF)
        {
            uint64_t pid = parentTF->_GetOwner()->_GetID();
            if (prefabIDs.find(pid) != prefabIDs.end())
            {
                parentID = pid;
            }
        }
        writer.Write("parentID", parentID);

        if (auto* tf = go->GetTransform())
        {
            writer.BeginObject("transform");
            writer.Write("id", tf->_GetID());
            tf->Serialize(writer);
            writer.EndObject();
        }

        writer.BeginArray("components");
        for (auto& comp : go->_GetComponents())
        {
            if (!comp) continue;
            writer.NextArrayItem();

            writer.Write("typeName", comp->_GetTypeName());
            writer.Write("id", comp->_GetID());
            comp->Serialize(writer);

            writer.EndArrayItem();
        }
        writer.EndArray();
        writer.EndArrayItem();
    }
    writer.EndArray();

    return writer.SaveFile(assetPath);
}

GameObject* ResourceManager::LoadModel(const std::string& fullPath)
{
    Assimp::Importer importer;

    // 옵션 설명:
    // aiProcess_Triangulate: 사각형 면을 삼각형으로 쪼갬 (필수)
    // aiProcess_ConvertToLeftHanded: DirectX 좌표계(왼손)로 변환 (필수)
    // aiProcess_GenSmoothNormals: 노말 벡터가 없으면 생성
    // aiProcess_CalcTangentSpace: 탄젠트/비탄젠트 생성 (노말맵용)
    // aiProcess_GlobalScale cm미터 단위로 저장되는걸 1m로 변환해줌
    const aiScene* scene = importer.ReadFile(fullPath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace 
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return nullptr;
    }


    std::vector<Texture*> extractedTextures;
    extractedTextures.resize(scene->mNumMaterials, nullptr);

    fs::path modelParentPath = fs::path(fullPath).parent_path();

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* srcMat = scene->mMaterials[i];

        if (srcMat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString texPath;
            if (srcMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
            {
                fs::path texFullPath = modelParentPath / texPath.C_Str();

                Texture* pTexture = Load<Texture>(texFullPath.string());
                if (pTexture)
                {
                    extractedTextures[i] = pTexture;
                }
            }
        }
    }

    std::string fileName = std::filesystem::path(fullPath).stem().string();
    GameObject* rootObject = SceneManager::Instance().GetActiveScene()->CreateGameObject(fileName);

    ProcessNode(scene->mRootNode, scene, rootObject, fullPath, extractedTextures);

    return rootObject;
}

void ResourceManager::ProcessNode(aiNode* node, const aiScene* scene, GameObject* parentGO, const std::string& modelPath, const std::vector<Texture*>& textures)
{
    GameObject* currentGO = nullptr;

    if (node == scene->mRootNode)
    {
        currentGO = parentGO;
    }
    else
    {
        currentGO = SceneManager::Instance().GetActiveScene()->CreateGameObject(node->mName.C_Str());
        currentGO->GetTransform()->SetParent(parentGO->GetTransform());
    }

    Matrix transformMatrix = ConvertMatrix(node->mTransformation);

    Vector3 scale, pos;
    Quaternion rot;
    transformMatrix.Transpose().Decompose(scale, rot, pos);

    // DirectX -> 행우선
    // Assimp -> 열우선

	currentGO->GetTransform()->SetPosition(pos);
    currentGO->GetTransform()->SetRotationQuat(rot);
    currentGO->GetTransform()->SetScale(scale);

    static const std::string defaultMatPath = "Assets/Materials/Default.mat";
    static uint64_t defaultMatID = AssetDatabase::Instance().GetIDFromPath(defaultMatPath);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        unsigned int matIndex = scene->mMeshes[meshIndex]->mMaterialIndex;

        aiMesh* aiMesh = scene->mMeshes[meshIndex];
        aiMaterial* aiMat = scene->mMaterials[aiMesh->mMaterialIndex];

        Texture* assignedTexture = nullptr;
        if (matIndex < textures.size())
        {
            assignedTexture = textures[matIndex];
        }

        GameObject* targetGO = currentGO;
        if (i > 0)
        {
            targetGO = SceneManager::Instance().GetActiveScene()->CreateGameObject(currentGO->GetName() + "_Sub_" + std::to_string(i));
            targetGO->GetTransform()->SetParent(currentGO->GetTransform());
        }

        MeshRenderer* renderer = targetGO->AddComponent<MeshRenderer>();
        renderer->SetModelID(AssetDatabase::Instance().GetIDFromPath(modelPath));
        renderer->SetMeshIndex(meshIndex);

        if (defaultMatID != 0)
        {
            renderer->SetMaterialID(defaultMatID);

            Material* mat = renderer->GetMaterial();
            if (mat)
            {
                aiColor3D color(1.f, 1.f, 1.f);
                if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                {
                    mat->SetColor(Vector4(color.r, color.g, color.b, 1.0f));
                }

                aiString texPath;
                if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
                {
                    std::filesystem::path modelDir = std::filesystem::path(modelPath).parent_path();
                    std::string fullTexPath = (modelDir / texPath.C_Str()).string();

                    Texture* texture = ResourceManager::Instance().Load<Texture>(fullTexPath);
                    if (texture)
                    {
                        mat->SetTexture(0, texture);
                    }
                }
            }
            // else:
            // 모델에 텍스처가 없다면? -> 아무것도 하지 않음.
            // 인스턴싱을 굳이 하지 않고 공유 머터리얼(Default)의 텍스처를 그대로 사용.
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, currentGO, modelPath, textures);
    }
}

Mesh* ResourceManager::ProcessMesh(aiMesh* aiMesh, const aiScene* scene)
{
    // 정점 데이터 추출
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;

        // 위치 (Position)
        vertex.Pos.x = aiMesh->mVertices[i].x;
        vertex.Pos.y = aiMesh->mVertices[i].y;
        vertex.Pos.z = aiMesh->mVertices[i].z;

        // 노말 (Normal)
        if (aiMesh->HasNormals())
        {
            vertex.Normal.x = aiMesh->mNormals[i].x;
            vertex.Normal.y = aiMesh->mNormals[i].y;
            vertex.Normal.z = aiMesh->mNormals[i].z;
        }

        // 텍스처 좌표 (UV)
        if (aiMesh->mTextureCoords[0])
        {
            vertex.Texcoord.x = aiMesh->mTextureCoords[0][i].x;
            vertex.Texcoord.y = aiMesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.Texcoord = { 0.0f, 0.0f };
        }

        if (aiMesh->HasTangentsAndBitangents()&& aiMesh->HasNormals())
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

            float w = (d < 0.0f) ? 1.0f : -1.0f;

            vertex.Tangent = { T.x, T.y, T.z, w };
        }
        else
        {
            vertex.Tangent = { 0.0f, 0.0f, 0.0f, 1.0f };
           
        } 

       

        vertices.push_back(vertex);
    }

    // 인덱스 데이터 추출
    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    auto mesh = std::make_shared<Mesh>();
    mesh->CreateBuffers(vertices, indices);

    // 리소스 관리 차원에서 캐싱해두는 것이 좋음 (여기서는 생략하고 바로 리턴)
    // m_meshes[aiMesh->mName.C_Str()] = mesh; 

    // Mesh는 IResource를 상속받았으므로 포인터 관리 주의 (여기선 raw pointer 반환 가정)
    // 실제로는 ResourceManager::Load<Mesh> 구조에 맞춰야 함.
    // 여기서는 개념 설명을 위해 new를 사용하지만, 프로젝트 구조에 맞게 shared_ptr 등을 사용하세요.

    Mesh* rawMesh = new Mesh();
    rawMesh->CreateBuffers(vertices, indices);

    return rawMesh;
}


GameObject* ResourceManager::InstantiatePrefab(const std::string& fullPath)
{
    auto rd = JsonReader::LoadJson(fullPath);
    if (!rd) return nullptr;
    JsonReader& reader = *rd;

    Scene* scene = SceneManager::Instance().GetActiveScene();
    if (!scene) return nullptr;

    std::unordered_map<uint64_t, Component*> fileIDToNewComponent;
    std::unordered_map<uint64_t, GameObject*> fileIDToNewGameObject;
    std::vector<FixupTask> fixupList;
    std::vector<std::pair<GameObject*, uint64_t>> parentLinks;

    GameObject* rootObject = nullptr;

    if (reader.BeginArray("gameObjects"))
    {
        while (reader.NextArrayItem())
        {
            std::string goName = reader.ReadString("name", "GameObject");
            uint64_t fileGoID = reader.ReadUInt64("id", 0);
            uint64_t fileParentID = reader.ReadUInt64("parentID", 0);

            GameObject* newGO = scene->CreateGameObject(goName);

            fileIDToNewGameObject[fileGoID] = newGO;
            if (fileParentID != 0)
            {
                parentLinks.push_back({ newGO, fileParentID });
            }

            if (!rootObject) rootObject = newGO;

            newGO->SetTag(reader.ReadString("tag", "Untagged"));
            newGO->SetActive(reader.ReadBool("active", true));

            if (reader.BeginObject("transform"))
            {
                Transform* tf = newGO->GetTransform();
                uint64_t fileTfID = reader.ReadUInt64("id", 0);

                if (fileTfID != 0) fileIDToNewComponent[fileTfID] = tf;

                DeserializeComponentProperties(reader, tf, fixupList);
                reader.EndObject();
            }

            if (reader.BeginArray("components"))
            {
                while (reader.NextArrayItem())
                {
                    std::string typeName = reader.ReadString("typeName", "");
                    uint64_t fileCompID = reader.ReadUInt64("id", 0);

                    if (!typeName.empty())
                    {
                        Component* newComp = newGO->AddComponent(typeName);
                        if (newComp)
                        {
                            if (fileCompID != 0) fileIDToNewComponent[fileCompID] = newComp;
                            DeserializeComponentProperties(reader, newComp, fixupList);
                        }
                    }
                    reader.EndArrayItem();
                }
                reader.EndArray();
            }
            reader.EndArrayItem();
        }
        reader.EndArray();
    }

    for (auto& link : parentLinks)
    {
        GameObject* child = link.first;
        uint64_t parentFileID = link.second;

        if (fileIDToNewGameObject.find(parentFileID) != fileIDToNewGameObject.end())
        {
            GameObject* parent = fileIDToNewGameObject[parentFileID];
            child->GetTransform()->SetParent(parent->GetTransform());
        }
    }

    for (const auto& task : fixupList)
    {
        uint64_t targetFileID = task.targetID;
        Component* targetPtr = nullptr;

        if (targetFileID != 0)
        {
            auto it = fileIDToNewComponent.find(targetFileID);
            if (it != fileIDToNewComponent.end())
            {
                targetPtr = it->second;
            }
        }
        task.property.m_setter(task.targetObject, &targetPtr);
    }

    return rootObject;
}
