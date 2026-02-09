#pragma once
#include "IResource.h"
#include <vector>
#include <array>

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

    // 적 정보 같이 저장??
    static constexpr int MAX_ENEMIES = 3;
    static constexpr int NUM_PATHPOINTS = 8;

    struct EnemySpawn
    {
        bool enabled = false;
        int type = 0;
        bool isBoss = false;

        std::array<Vector2, NUM_PATHPOINTS> pathPoints;

        EnemySpawn()
        {
            for (auto& p : pathPoints)
            {
                p = Vector2{ -999.f, -999.f };
            }
        }
    };

    EnemySpawn& GetEnemy(int idx) { return m_enemies[idx]; }
    const EnemySpawn& GetEnemy(int idx) const { return m_enemies[idx]; }


    // 고양이 대사도??
    //static constexpr int MAX_DIALOGUES = 3;

    //struct DialogueLine
    //{
    //    bool enabled = false;
    //    std::string text;
    //};

    //DialogueLine& GetDialogue(int idx) { return m_dialogues[idx]; }
    //const DialogueLine& GetDialogue(int idx) const { return m_dialogues[idx]; }

    // 앨리스도???
    const Vector2& GetAlicePos() const { return m_alicePos; }
    void SetAlicePos(const Vector2& p) { m_alicePos = p; }


    // 붉은 여왕도????
    struct RedQueenSpawn
    {
        bool enabled = false;
        Vector2 pos = Vector2{ -999.f, -999.f };
    };

    const RedQueenSpawn& GetRedQueen() const { return m_redQueen; }
    RedQueenSpawn& GetRedQueen() { return m_redQueen; }

    int FindDefaultGrid(int x, int y) const;

private:
    void MakeBorder(); 

    bool IsCorner(int x, int y) const;
    bool IsBorder(int x, int y) const;

    void ResetEnemiesToDefault();
    //void ResetDialoguesToDefault();
    void ResetSpecialsToDefault(); // 앨리스랑 붉은여왕 묶어서.

private:
    int m_width = 12; // 이건 테두리 포함한 값임.
    int m_height = 12;

    std::vector<int> m_grid;

    std::array<EnemySpawn, MAX_ENEMIES> m_enemies;
    //std::array<DialogueLine, MAX_DIALOGUES> m_dialogues;

    Vector2 m_alicePos = Vector2{ -999.f, -999.f };

    RedQueenSpawn m_redQueen;
};

// 