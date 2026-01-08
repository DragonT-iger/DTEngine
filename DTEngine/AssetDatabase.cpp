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
        ".vcxproj", ".filters", ".user" , ".orig"
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

	std::string normalizedAssetPath = NormalizePath(assetPath);

    if (fs::exists(metaPath))
    {
        id = ReadMetaFile(metaPath);

		//std::cout << "Loaded .meta for " << normalizedAssetPath << " with ID: " << id << std::endl;
    }
    else
    {
        id = CreateMetaFile(normalizedAssetPath);
    }

    if (id != 0)
    {
        assert(m_idToPathMap.find(id) == m_idToPathMap.end() && "Duplicate asset ID detected!");

        m_idToPathMap[id] = normalizedAssetPath;
     
        
        m_pathToIdMap[normalizedAssetPath] = id;
    }

    else {
        std::cout << "말도 안되는 레전드 상황 발생" << std::endl;
    }
}

bool AssetDatabase::RenameAsset(const std::string& oldPath, const std::string& newName)
{
    fs::path pOld(oldPath);
    fs::path pNew = pOld.parent_path() / newName;

    if (oldPath == pNew.string()) return false;
    if (fs::exists(pNew))
    {
        std::cerr << "[AssetDatabase] Rename Failed: Target already exists." << std::endl;
        return false;
    }

    try {
        fs::rename(pOld, pNew);
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << "[AssetDatabase] FS Error: " << e.what() << std::endl;
        return false;
    }

    fs::path pOldMeta = pOld.string() + ".meta";
    fs::path pNewMeta = pNew.string() + ".meta";
    if (fs::exists(pOldMeta))
    {
        fs::rename(pOldMeta, pNewMeta);
    }

    uint64_t id = GetIDFromPath(oldPath);
    if (id != 0)
    {
        std::string normalizedOld = NormalizePath(oldPath);
        m_pathToIdMap.erase(normalizedOld);

        std::string normalizedNew = NormalizePath(pNew.string());
        m_idToPathMap[id] = normalizedNew;
        m_pathToIdMap[normalizedNew] = id;
    }

    return true;
}

uint64_t AssetDatabase::ReadMetaFile(const std::string& metaPath)
{
    auto readerOpt = JsonReader::LoadJson(metaPath);
    if (!readerOpt)
    {
        return 0; 
    }

    JsonReader& reader = *readerOpt;
    return reader.ReadUInt64("FileID", 0);
}

uint64_t AssetDatabase::CreateMetaFile(const std::string& assetPath)
{
    std::string metaPath = assetPath + ".meta";

    uint64_t newID = IDManager::Instance().GetNewUniqueID();

    JsonWriter writer; 
    writer.Write("FileID", newID); 
     
    if (writer.SaveFile(metaPath)) 
    {
        //std::cout << "Created .meta for " << assetPath << std::endl;
        return newID;
    }

    return 0; // false
}

std::string AssetDatabase::NormalizePath(const std::string& path) const
{
	std::string pathCopy = path;
    std::replace(pathCopy.begin(), pathCopy.end(), '\\', '/');
    return pathCopy;
}


uint64_t AssetDatabase::GetIDFromPath(const std::string& path) const
{
    std::string normalizedAssetPath = NormalizePath(path);

    auto it = m_pathToIdMap.find(normalizedAssetPath);
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
