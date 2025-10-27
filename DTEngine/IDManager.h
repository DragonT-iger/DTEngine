#pragma once
#include "Singleton.h"
#include <cstdint>
#include <random>

class IDManager : public Singleton<IDManager>
{
    friend class Singleton;
public:
    uint64_t GetNewUniqueID()
    {
        return m_distribution(m_generator);
    }

private:
    IDManager()
        : m_generator(std::random_device{}()),
        m_distribution(1, std::numeric_limits<uint64_t>::max())
    {
    }
    ~IDManager() = default;

    std::mt19937_64 m_generator;
    std::uniform_int_distribution<uint64_t> m_distribution;
};