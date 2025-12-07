#pragma once
#include "Singleton.h"

#include <string>
#include <unordered_map>

#include "IResource.h"
#include "AssetDatabase.h"

class DX11Renderer;
class GameObject;
struct aiNode;
struct aiScene;
struct aiMesh;
class Mesh;
class Material;
class Texture;

// 한번 로드된 리소스는 캐싱된다.

class ResourceManager : public Singleton<ResourceManager>
{
public:
	bool Initialize(const std::string& resourceRootPath);

	template<typename T>
	T* Load(const std::string& filePath);

	void Unload(const std::string& id);
	void UnloadAll();

    std::string ResolveFullPath(const std::string& path) const;

	std::string GetResourceRootPath() const { return m_resourceRootPath; }

    GameObject* InstantiatePrefab(const std::string& fullPath);
    bool SavePrefab(GameObject* root, const std::string& fullPath);

    GameObject* LoadModel(const std::string& fullPath);

private:
    void ProcessNode(aiNode* node, const aiScene* scene, GameObject* parentGO, const std::string& modelPath, const std::vector<Texture*>& textures);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);

    //uint64_t ProcessMaterial(const aiScene* scene, unsigned int materialIndex, const std::string& modelPath);

    void CollectDescendants(GameObject* target, std::vector<GameObject*>& outList);

	std::string m_resourceRootPath;
	std::unordered_map<std::string, std::unique_ptr<IResource>> m_cache;
};

template<typename T>
inline T* ResourceManager::Load(const std::string& filePath)
{
    static_assert(std::is_base_of<IResource, T>::value, "T must derive from IResource");

    if (auto it = m_cache.find(filePath); it != m_cache.end())
        return dynamic_cast<T*>(it->second.get());

    const std::string fullPath = ResolveFullPath(filePath);

    auto res = std::make_unique<T>();
    res->SetMeta(ResourceMeta{ AssetDatabase::Instance().GetIDFromPath(fullPath) });

    if (!res->LoadFile(fullPath))
        return nullptr;

    T* raw = res.get();
    m_cache.emplace(filePath, std::move(res));
    return raw;
}