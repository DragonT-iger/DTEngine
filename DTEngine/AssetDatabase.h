#pragma once
#include "Singleton.h"
#include <cstdint>
#include <string>
#include <unordered_map>

// ResourceManager�� �Ʒ��ܿ��� �����ϴ� ID ���� �����ͺ��̽�

class AssetDatabase : public Singleton<AssetDatabase>
{
    friend class Singleton<AssetDatabase>;

public:
    bool Initialize();

    uint64_t GetIDFromPath(const std::string& path) const;

    std::string GetPathFromID(uint64_t id) const;

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    void ScanDirectory(const std::string& directoryPath);
    void ProcessAssetFile(const std::string& assetPath);

    uint64_t ReadMetaFile(const std::string& metaPath);
    uint64_t CreateMetaFile(const std::string& assetPath); // �̰͵� ��ũ�� �Գ� �ٵ� ������ ��

    std::string m_assetRoot;

    std::unordered_map<uint64_t, std::string> m_idToPathMap;
    std::unordered_map<std::string, uint64_t> m_pathToIdMap;
};