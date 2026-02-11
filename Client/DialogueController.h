#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <string>

class GameObject;
class Text;

class DialogueController : public MonoBehaviour
{
    DTGENERATED_BODY(DialogueController);

public:
    virtual void Start() override;
    virtual void Update(float deltaTime) override;

private:
    void NextDialogue();

private:
    GameObject* m_aliceObject = nullptr;    
    GameObject* m_redQueenObject = nullptr; 

    Text* m_dialogueText = nullptr;
    Text* m_redQueenText = nullptr;

    std::vector<std::wstring> m_dialogueLines;

    int m_currentIndex = -1;
};