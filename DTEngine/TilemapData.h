#pragma once
#include "IResource.h"
#include <vector>

class TilemapData : public IResource
{
public:
    TilemapData() = default;
    virtual ~TilemapData() = default;

    static constexpr float TILE_SIZE = 1.0f;

    virtual bool LoadFile(const std::string& fullPath) override;
    virtual bool SaveFile(const std::string& fullPath) override;
    virtual void Unload() override;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

	void SetWidth(int w) { m_width = w; SetDimensions(m_width, m_height); }
	void SetHeight(int h) { m_height = h; SetDimensions(m_width, m_height); }

    void SetDimensions(int w, int h);
    void SetTileIndex(int x, int y, int paletteIndex);
    int GetTileIndex(int x, int y) const;

private:
    int m_width = 10;
    int m_height = 10;

    std::vector<int> m_grid;
};