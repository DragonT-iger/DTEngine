#pragma once
#include "Singleton.h"
#include <cstdint>
#include <memory>   

class IDManager : public Singleton<IDManager> 
{
    friend class Singleton<IDManager>;

public:
    ~IDManager();

    uint64_t GetNewUniqueID();

private:
    IDManager(); 

    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};