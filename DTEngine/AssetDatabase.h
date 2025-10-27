#pragma once
#include "Singleton.h"
#include <cstdint>
#include <string>
#include <unordered_map>

class AssetDatabase : public Singleton<AssetDatabase>
{
    friend class Singleton<AssetDatabase>;

public:
    // 엔진 시작 시 호출: Assets/ 폴더를 스캔하고 .meta 파일 관리
    void Initialize(const std::string& assetRootPath);

    // 경로로 ID 찾기 (예: "Assets/Models/Hero.fbx")
    uint64_t GetIDFromPath(const std::string& path) const;

    // ID로 경로 찾기 (예: 1234567890ABCDEF)
    std::string GetPathFromID(uint64_t id) const;

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    // .meta 파일을 읽거나 생성하는 메인 로직
    void ScanDirectory(const std::string& directoryPath);
    void ProcessAssetFile(const std::string& assetPath);

    // .meta 파일 파싱
    uint64_t ReadMetaFile(const std::string& metaPath);
    // .meta 파일 생성
    uint64_t CreateMetaFile(const std::string& assetPath);

    std::string m_assetRoot;

    // 핵심 맵: ID와 경로를 양방향으로 매핑
    std::unordered_map<uint64_t, std::string> m_idToPathMap;
    std::unordered_map<std::string, uint64_t> m_pathToIdMap;
};