#pragma once
#include "IResource.h"
#include <vector>

class TilemapData : public IResource
{
public:
    TilemapData() = default;
    virtual ~TilemapData() = default;

    static constexpr float TILE_SIZE = 2.0f;

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    int GetWidth()  const { return (std::max)(0, m_width - 2); } // 테두리 안포함
    int GetHeight() const { return (std::max)(0, m_height - 2); }
    int GetExpandedWidth()  const { return m_width; } // 테두리 포함
    int GetExpandedHeight() const { return m_height; }

    void SetWidth(int innerW) { SetDimensions(innerW, GetHeight()); } // 테두리 안포함
    void SetHeight(int innerH) { SetDimensions(GetWidth(), innerH); }

    void SetDimensions(int w, int h); // 이건 테두리 포함하지 않은 값 사용해야함.
    void SetTileIndex(int x, int y, int paletteIndex);
    int GetTileIndex(int x, int y) const;

private:
    int FindDefaultGrid(int x, int y) const;
    void MakeBorder(); 

    bool IsCorner(int x, int y) const;
    bool IsBorder(int x, int y) const;

private:
    int m_width = 12; // 이건 테두리 포함한 값임.
    int m_height = 12;

    std::vector<int> m_grid;
};

// 