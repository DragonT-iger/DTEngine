#pragma once
#include "IResource.h"
#include <wrl/client.h> 
#include <string>

enum class FilterMode { Point, Bilinear, Trilinear };
enum class WrapMode { Repeat, Clamp };

struct ID3D11ShaderResourceView;
struct ID3D11Resource;
struct ID3D11SamplerState;

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

    ID3D11SamplerState* GetSampler() const { return m_currentSampler; }

    FilterMode GetFilterMode() const { return m_filterMode; }
    void SetFilterMode(FilterMode mode);

    WrapMode GetWrapMode() const { return m_wrapMode; }
    void SetWrapMode(WrapMode mode);

    void SaveImportSettings(const std::string& fullPath);

protected:
    void UpdateSampler();
    void LoadMetaData(const std::string& fullPath);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11Resource> m_textureResource;

    int m_width = 0;
    int m_height = 0;

    ID3D11SamplerState* m_currentSampler = nullptr; // DX11Renderer에서 미리 6개 만들어 놓은거 참조만 함

    FilterMode m_filterMode = FilterMode::Bilinear;
    WrapMode m_wrapMode = WrapMode::Repeat;
};