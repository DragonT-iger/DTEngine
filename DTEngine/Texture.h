#pragma once
#include "IResource.h"
#include <wrl/client.h> 
#include <string>

struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class Texture : public IResource
{
public:
    Texture();
    virtual ~Texture();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    ID3D11ShaderResourceView* GetSRV() const { return m_srv.Get(); }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11Resource> m_textureResource;

    int m_width = 0;
    int m_height = 0;
};