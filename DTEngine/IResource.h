#pragma once
#include <string>

class DX11Renderer;

struct ResourceMeta {
    std::string filePath;
};

class IResource {
public:
    virtual ~IResource() = default;

    virtual bool LoadFile(const std::string& fullPath) = 0;
    virtual bool SaveFile(const std::string& fullPath) = 0;
    virtual void Unload() = 0;

    const ResourceMeta& GetMeta() const { return m_meta; }
    void SetMeta(const ResourceMeta& m) { m_meta = m; }

private:
    ResourceMeta m_meta;
};
