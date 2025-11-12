#pragma once
#include "Singleton.h"
#include "ICommand.h"
#include <deque>
#include <memory>

class HistoryManager : public Singleton<HistoryManager>
{
public:
    static constexpr size_t MAX_HISTORY_COUNT = 100;

    void Do(std::unique_ptr<ICommand> cmd)
    {
        cmd->Execute();

        m_undoStack.push_back(std::move(cmd));

        while (m_undoStack.size() > MAX_HISTORY_COUNT)
        {
            m_undoStack.pop_front();
        }

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
    std::deque<std::unique_ptr<ICommand>> m_undoStack;
    std::deque<std::unique_ptr<ICommand>> m_redoStack;
};