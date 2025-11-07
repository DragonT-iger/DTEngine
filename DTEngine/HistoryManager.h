#pragma once
#include "Singleton.h"
#include "ICommand.h"
#include <vector>
#include <memory>

class HistoryManager : public Singleton<HistoryManager>
{
public:
    void Do(std::unique_ptr<ICommand> cmd)
    {
        cmd->Execute();

        m_undoStack.push_back(std::move(cmd));

        m_redoStack.clear();
    }

    void Undo()
    {
        if (m_undoStack.empty()) return;

        std::unique_ptr<ICommand> cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();

        cmd->Undo();

        m_redoStack.push_back(std::move(cmd));
    }

    void Redo()
    {
        if (m_redoStack.empty()) return;

        std::unique_ptr<ICommand> cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();

        cmd->Execute();

        m_undoStack.push_back(std::move(cmd));
    }

private:
    std::vector<std::unique_ptr<ICommand>> m_undoStack;
    std::vector<std::unique_ptr<ICommand>> m_redoStack;
};