#include "pch.h"
#include "IDManager.h"
#include <random>
#include <limits>

struct IDManager::Impl
{
    std::mt19937_64 m_generator;
    std::uniform_int_distribution<uint64_t> m_distribution;

    Impl()
        : m_generator(std::random_device{}()),
        m_distribution(1, std::numeric_limits<uint64_t>::max())
    {
    }
};

IDManager::IDManager()
    : m_pImpl(std::make_unique<Impl>())
{
}
IDManager::~IDManager() = default;

uint64_t IDManager::GetNewUniqueID()
{
    return m_pImpl->m_distribution(m_pImpl->m_generator);
}