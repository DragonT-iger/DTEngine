#include "pch.h"
#include "TilemapData.h"
#include "JsonIO.h"
#include "GameObject.h"
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
            if (!IsBorder(x, y))
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

    // 적
    ResetEnemiesToDefault();

    if (reader.BeginArray("enemies"))
    {
        int ei = 0;
        while (reader.NextArrayItem())
        {
            if (ei < MAX_ENEMIES)
            {
                auto& e = m_enemies[ei];
                e.enabled = reader.ReadBool("enabled", false);
                e.type = reader.ReadInt("type", 0);
                e.isBoss = reader.ReadBool("isBoss", false);

                // pathPoints
                if (reader.BeginArray("pathPoints"))
                {
                    int pi = 0;
                    while (reader.NextArrayItem())
                    {
                        if (pi < NUM_PATHPOINTS)
                        {
                            float x = reader.ReadFloat("x", -999.f);
                            float y = reader.ReadFloat("y", -999.f);
                            e.pathPoints[pi] = Vector2{ x, y };
                        }
                        pi++;
                        reader.EndArrayItem();
                    }
                    reader.EndArray();
                }
            }

            ei++;
            reader.EndArrayItem();
        }
        reader.EndArray();
    }

    // 대사
   /* ResetDialoguesToDefault();

    if (reader.BeginArray("dialogues"))
    {
        int di = 0;
        while (reader.NextArrayItem())
        {
            if (di < MAX_DIALOGUES)
            {
                auto& d = m_dialogues[di];
                d.enabled = reader.ReadBool("enabled", false);
                d.text = reader.ReadString("text", "");
            }
            di++;
            reader.EndArrayItem();
        }
        reader.EndArray();
    }*/

    ResetSpecialsToDefault();

    // 앨리스
    if (reader.BeginObject("alice"))
    {
        float x = reader.ReadFloat("x", -999.f);
        float y = reader.ReadFloat("y", -999.f);
        m_alicePos = Vector2{ x, y };
        reader.EndObject();
    }

    // 붉은여왕
    if (reader.BeginObject("redQueen"))
    {
        m_redQueen.enabled = reader.ReadBool("enabled", false);
        float x = reader.ReadFloat("x", -999.f);
        float y = reader.ReadFloat("y", -999.f);
        m_redQueen.pos = Vector2{ x, y };
        reader.EndObject();
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

    // 적 저장
    writer.BeginArray("enemies");
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        const auto& e = m_enemies[i];
        writer.NextArrayItem();

        writer.Write("enabled", e.enabled);
        writer.Write("type", e.type);
        writer.Write("isBoss", e.isBoss);

        writer.BeginArray("pathPoints");
        for (int p = 0; p < NUM_PATHPOINTS; ++p)
        {
            writer.NextArrayItem();
            writer.Write("x", e.pathPoints[p].x);
            writer.Write("y", e.pathPoints[p].y);
            writer.EndArrayItem();
        }
        writer.EndArray();

        writer.EndArrayItem();
    }
    writer.EndArray();

    // 대사 저장
   /* writer.BeginArray("dialogues");
    for (int i = 0; i < MAX_DIALOGUES; ++i)
    {
        const auto& d = m_dialogues[i];
        writer.NextArrayItem();
        writer.Write("enabled", d.enabled);
        writer.Write("text", d.text);
        writer.EndArrayItem();
    }
    writer.EndArray();*/

    // 앨리스
    writer.BeginObject("alice");
    writer.Write("x", m_alicePos.x);
    writer.Write("y", m_alicePos.y);
    writer.EndObject();

    // 붉은여왕
    writer.BeginObject("redQueen");
    writer.Write("enabled", m_redQueen.enabled);
    writer.Write("x", m_redQueen.pos.x);
    writer.Write("y", m_redQueen.pos.y);
    writer.EndObject();

    return writer.SaveFile(fullPath);
}

void TilemapData::Unload()
{
    m_grid.clear();
    ResetEnemiesToDefault();
    //ResetDialoguesToDefault();
    ResetSpecialsToDefault();
}


int TilemapData::FindDefaultGrid(int x, int y) const
{
    int ix = x - 1;
    int iy = y - 1;
    return ((ix + iy) % 2 == 0) ? 0 : 1;
}

void TilemapData::MakeBorder()
{
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            if (!IsBorder(x, y))
                continue;

            if (IsCorner(x, y))
            {
                m_grid[y * m_width + x] = 10; // 꼭짓점
            }
            else
            {
                // 8 9 번갈아
                m_grid[y * m_width + x] = ((x + y) % 2 == 0) ? 8 : 9;
            }
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



void TilemapData::ResetEnemiesToDefault()
{
    for (auto& e : m_enemies)
    {
        e = EnemySpawn{};
    }

}

void TilemapData::ResetSpecialsToDefault()
{
    m_alicePos = Vector2{ -999.f, -999.f };
    m_redQueen.enabled = false;
    m_redQueen.pos = Vector2{ -999.f, -999.f };
}

//void TilemapData::ResetDialoguesToDefault()
//{
//    for (auto& d : m_dialogues)
//    {
//        d.enabled = false;
//        d.text.clear();
//    }
//}
