#include "pch.h"
#include "ResourceManager.h"

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

std::string ResourceManager::ResolveFullPath(const std::string& id) const
{
	if (m_resourceRootPath.empty()) return id;

	const char last = m_resourceRootPath.back();
	if (last == '/' || last == '\\')
		return m_resourceRootPath + id;

	return m_resourceRootPath + "\\" + id;
}
