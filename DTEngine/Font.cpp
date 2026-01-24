#include "pch.h"
#include <DirectXTK/SpriteFont.h>
#include "Font.h"
#include  "ResourceManager.h"
#include "DX11Renderer.h"


Font::~Font()
{
    Unload();
}

bool Font::LoadFile(const std::string& fullPath)
{
  //얘는 그냥 SpriteFont를 fullpath를 통해서 찾아서, device를 통해서 만들면 되는건가


    auto device = DX11Renderer::Instance().GetDevice();
    std::wstring wPath(fullPath.begin(), fullPath.end());

    try {
        m_spriteFont = new DirectX::DX11::SpriteFont(device, wPath.c_str());
        return true;
    }
    catch (...) {
        return false;
    }

}

bool Font::SaveFile(const std::string& fullPath)
{
    return false;
}


void Font::Unload()
{
    delete m_spriteFont;
    m_spriteFont = nullptr;
}

//원본 포인터는 ResourceManager에서 