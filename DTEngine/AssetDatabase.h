#pragma once
#include "Singleton.h"
#include <cstdint>
#include <string>
#include <unordered_map>

class AssetDatabase : public Singleton<AssetDatabase>
{
    friend class Singleton<AssetDatabase>;

public:
    // ���� ���� �� ȣ��: Assets/ ������ ��ĵ�ϰ� .meta ���� ����
    void Initialize(const std::string& assetRootPath);

    // ��η� ID ã�� (��: "Assets/Models/Hero.fbx")
    uint64_t GetIDFromPath(const std::string& path) const;

    // ID�� ��� ã�� (��: 1234567890ABCDEF)
    std::string GetPathFromID(uint64_t id) const;

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    // .meta ������ �аų� �����ϴ� ���� ����
    void ScanDirectory(const std::string& directoryPath);
    void ProcessAssetFile(const std::string& assetPath);

    // .meta ���� �Ľ�
    uint64_t ReadMetaFile(const std::string& metaPath);
    // .meta ���� ����
    uint64_t CreateMetaFile(const std::string& assetPath);

    std::string m_assetRoot;

    // �ٽ� ��: ID�� ��θ� ��������� ����
    std::unordered_map<uint64_t, std::string> m_idToPathMap;
    std::unordered_map<std::string, uint64_t> m_pathToIdMap;
};