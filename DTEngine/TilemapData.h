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

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

	void SetWidth(int w) {  SetDimensions(w, m_height); }
	void SetHeight(int h) { SetDimensions(m_width, h); }

    void SetDimensions(int w, int h);
    void SetTileIndex(int x, int y, int paletteIndex);
    int GetTileIndex(int x, int y) const;

private:
    int m_width = 10;
    int m_height = 10;

    std::vector<int> m_grid;
};