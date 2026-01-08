#pragma once
#include "Singleton.h"
#include <cstdint>
#include <string>
#include <unordered_map>

// ResourceManager의 아래단에서 동작하는 ID 관리 데이터베이스

class AssetDatabase : public Singleton<AssetDatabase>
{
    friend class Singleton<AssetDatabase>;

public:
    bool Initialize();

    uint64_t GetIDFromPath(const std::string& path) const;

    std::string GetPathFromID(uint64_t id) const;

	void ProcessAssetFile(const std::string& assetPath); // 새로운 에셋 파일이 추가되었을 때 호출

    bool RenameAsset(const std::string& oldPath, const std::string& newName);

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    void ScanDirectory(const std::string& directoryPath);

    uint64_t ReadMetaFile(const std::string& metaPath);
    uint64_t CreateMetaFile(const std::string& assetPath); // 이것도 매크로 먹네 근데 실행은 됨
    
    std::string NormalizePath(const std::string& path) const;

    std::string m_assetRoot;

    std::unordered_map<uint64_t, std::string> m_idToPathMap;
    std::unordered_map<std::string, uint64_t> m_pathToIdMap;
};