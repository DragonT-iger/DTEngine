#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <string>

class GameObject;
class Text;
class TilemapGenerator;
class Tilemap;

enum class Speaker
{
    Alice,
    RedQueen,
    Cheshire
};

struct DialogueData
{
    Speaker speaker;  
    std::wstring text;
    std::function<void()> onStartAction = nullptr;
};

class DialogueController : public MonoBehaviour
{
    DTGENERATED_BODY(DialogueController);

public:
    void Start() override;
    void Update(float deltaTime) override;

private:
    void NextDialogue();

private:
    GameObject* m_aliceObject = nullptr;
    Text* m_dialogueText = nullptr;         

    GameObject* m_redQueenObject = nullptr;
    Text* m_redQueenText = nullptr;         

    GameObject* m_cheshireObject = nullptr;
    Text* m_cheshireText = nullptr;

    std::vector<DialogueData> m_dialogueLines;

    TilemapGenerator* m_tilemapGenerator5 = nullptr;
    TilemapGenerator* m_tilemapGenerator6 = nullptr;


    int m_currentIndex = -1;
};