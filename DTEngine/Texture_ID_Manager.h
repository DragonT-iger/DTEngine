#pragma once

#include <vector>
#include <map>

class TextureCombinationManager
{
public:
    static uint16_t GetID(const std::vector<uint64_t>& textureGUIDs)
    {
        static std::map<std::vector<uint64_t>, uint16_t> combinationRegistry;
        static uint16_t nextAvailableID = 0;
        auto it = combinationRegistry.find(textureGUIDs);

        if (it != combinationRegistry.end())
        {
            return it->second;
        }
        uint16_t newID = ++nextAvailableID; 
        combinationRegistry[textureGUIDs] = newID;

        return newID;
    }
};