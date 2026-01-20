#include "pch.h"
#include "TilemapData.h"
#include "JsonIO.h"


void TilemapData::SetDimensions(int w, int h)
{
    m_width = w;
    m_height = h;
    m_grid.assign(w * h, -1);
}

void TilemapData::SetTileIndex(int x, int y, int paletteIndex)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
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

    m_width = reader.ReadInt("width", 10);
    m_height = reader.ReadInt("height", 10);

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