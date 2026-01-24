#pragma once 
#include "IResource.h"

namespace DirectX
{
    namespace DX11
    {
        class SpriteFont;

    }
}

class Font : public IResource
{
public:
    Font() = default;
    virtual ~Font(); // 소멸자에서 delete를 하려면 .cpp에서 헤더를 포함해야 함

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload();

    // 포인터나 참조자로 사용
    DirectX::DX11::SpriteFont* GetSpriteFont() const { return m_spriteFont; }

private:
    DirectX::DX11::SpriteFont* m_spriteFont = nullptr;
};