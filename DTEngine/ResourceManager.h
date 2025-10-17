#pragma once
#include "Singleton.h"
#include <string>
#include <unordered_map>
#include "IResource.h"

class DX11Renderer;

// �ѹ� �ε�� ���ҽ��� ĳ�̵ȴ�.

class ResourceManager : public Singleton<ResourceManager>
{
public:
	bool Initalize(const std::string& resourceRootPath);

	template<typename T>
	T* Load(const std::string& filePath);

	void Unload(const std::string& id);
	void UnloadAll();

private:
	std::string ResolveFullPath(const std::string& id) const;

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
    res->SetMeta(ResourceMeta{ filePath });

    if (!res->LoadFromFile(fullPath))
        return nullptr;

    T* raw = res.get();
    m_cache.emplace(filePath, std::move(res));
    return raw;
}