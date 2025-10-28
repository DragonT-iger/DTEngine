#include "pch.h"
#include "AssetDatabase.h"
#include "IDManager.h"
#include "JsonIO.h" 
#include <filesystem> 
#include <iostream>
#include <set>
#include "ResourceManager.h"
namespace fs = std::filesystem;

bool AssetDatabase::Initialize()
{
	m_assetRoot = ResourceManager::Instance().GetResourceRootPath();
    ScanDirectory(m_assetRoot);
    std::cout << "AssetDatabase: " << m_idToPathMap.size() << " assets loaded." << std::endl;

    return true;
}


//파일 이름 ignore 
void AssetDatabase::ScanDirectory(const std::string& directoryPath)
{
    static const std::set<std::string> ignoredExtensions = {
        ".meta", ".h", ".cpp",
        ".vcxproj", ".filters", ".user"
    };
    static const std::set<std::string> ignoredFileNames = {
        "packages.config"
    };

    for (auto it = fs::recursive_directory_iterator(directoryPath); it != fs::end(it);
        ++it)
    {
        const auto& entry = *it;

        //if (entry.is_directory())
        //{
        //    if (entry.path().filename() == "x64")
        //    {
        //        it.disable_recursion_pending();
        //    }
        //    continue;
        //}

        if (!entry.is_regular_file())
        {
            continue;
        }

        const auto& path = entry.path();

        if (ignoredFileNames.count(path.filename().string()))
        {
            continue;
        }

        if (ignoredExtensions.count(path.extension().string()))
        {
            continue;
        }

        ProcessAssetFile(path.string());
    }
}

// .meta 파일이 있는지 확인하고 처리
void AssetDatabase::ProcessAssetFile(const std::string& assetPath)
{
    std::string metaPath = assetPath + ".meta";
    uint64_t id = 0;

    if (fs::exists(metaPath))
    {
        id = ReadMetaFile(metaPath);

		std::cout << "Loaded .meta for " << assetPath << " with ID: " << id << std::endl;
    }
    else
    {
        id = CreateMetaFile(assetPath);
    }

    if (id != 0)
    {
        m_idToPathMap[id] = assetPath;
        m_pathToIdMap[assetPath] = id;
    }
}

uint64_t AssetDatabase::ReadMetaFile(const std::string& metaPath)
{
    auto readerOpt = JsonReader::LoadJson(metaPath);
    if (!readerOpt)
    {
        return 0; 
    }

    JsonReader& reader = *readerOpt;
    return reader.ReadUInt64("guid", 0);
}

uint64_t AssetDatabase::CreateMetaFile(const std::string& assetPath)
{
    std::string metaPath = assetPath + ".meta";

    uint64_t newID = IDManager::Instance().GetNewUniqueID();

    JsonWriter writer; 
    writer.Write("guid", newID); 

    if (writer.SaveFile(metaPath)) 
    {
        std::cout << "Created .meta for " << assetPath << std::endl;
        return newID;
    }

    return 0; // false
}


uint64_t AssetDatabase::GetIDFromPath(const std::string& path) const
{
    auto it = m_pathToIdMap.find(path);
    if (it != m_pathToIdMap.end())
    {
        return it->second;
    }
    return 0; // 탐색 실패
}

std::string AssetDatabase::GetPathFromID(uint64_t id) const
{
    auto it = m_idToPathMap.find(id);
    if (it != m_idToPathMap.end())
    {
        return it->second;
    }
    return ""; // 경로 없음
}
