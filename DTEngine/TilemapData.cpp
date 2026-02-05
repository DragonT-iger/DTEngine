#include "pch.h"
#include "TilemapData.h"
#include "JsonIO.h"
#include <algorithm>

void TilemapData::SetDimensions(int w, int h)
{
    // w,h 는 안쪽 크기
    w = std::max(0, w);
    h = std::max(0, h);

    int expandedW = w + 2;
    int expandedH = h + 2;

    if (expandedW == m_width && expandedH == m_height)
        return;

    std::vector<int> oldGrid = m_grid;
    int oldW = m_width;
    int oldH = m_height;

    m_width = expandedW;
    m_height = expandedH;
    m_grid.assign(m_width * m_height, -1);

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            if (IsBorder(x, y))
                m_grid[y * m_width + x] = IsCorner(x, y) ? 9 : 8;
            else
                m_grid[y * m_width + x] = FindDefaultGrid(x, y);
        }
    }

    if (!oldGrid.empty() && oldW > 2 && oldH > 2)
    {
        int copyW = std::min(oldW, m_width);
        int copyH = std::min(oldH, m_height);

        for (int y = 1; y < copyH - 1; ++y)
        {
            for (int x = 1; x < copyW - 1; ++x)
            {
                m_grid[y * m_width + x] = oldGrid[y * oldW + x];
            }
        }
    }

    MakeBorder();
}

void TilemapData::SetTileIndex(int x, int y, int paletteIndex)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || IsBorder(x, y)) return;
    m_grid[y * m_width + x] = paletteIndex;
}

int TilemapData::GetTileIndex(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return -1;
    return m_grid[y * m_width + x];
}

bool TilemapData::LoadFile(const std::string& fullPath)
{
    auto readerOpt = JsonReader::LoadJson(fullPath);
    if (!readerOpt) return false;
    JsonReader& reader = *readerOpt;

    m_width = reader.ReadInt("width", 12);
    m_height = reader.ReadInt("height", 12);

    m_grid.clear();
    int size = m_width * m_height;
    if (size > 0) m_grid.resize(size, -1);

    if (reader.BeginArray("grid"))
    {
        int index = 0;
        while (reader.NextArrayItem())
        {
            if (index < m_grid.size())
                m_grid[index] = reader.ReadInt("v", -1);
            index++;
            reader.EndArrayItem();
        }
        reader.EndArray();
    }

    MakeBorder();
    return true;
}

bool TilemapData::SaveFile(const std::string& fullPath)
{
    JsonWriter writer;
    writer.Write("width", m_width);
    writer.Write("height", m_height);

    writer.BeginArray("grid");
    for (int v : m_grid)
    {
        writer.NextArrayItem();
        writer.Write("v", v);
        writer.EndArrayItem();
    }
    writer.EndArray();

    return writer.SaveFile(fullPath);
}

void TilemapData::Unload()
{
    m_grid.clear();
}


int TilemapData::FindDefaultGrid(int x, int y) const
{
    int ix = x - 1;
    int iy = y - 1;
    return ((ix + iy) % 2 == 0) ? 2 : 3;
}

void TilemapData::MakeBorder()
{
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            if (!IsBorder(x, y)) continue;
            m_grid[y * m_width + x] = IsCorner(x, y) ? 9 : 8;
        }
    }
}

bool TilemapData::IsCorner(int x, int y) const
{
    return (x == 0 || x == m_width - 1) && (y == 0 || y == m_height - 1);
}

bool TilemapData::IsBorder(int x, int y) const
{
    return (x == 0 || y == 0 || x == m_width - 1 || y == m_height - 1);
}