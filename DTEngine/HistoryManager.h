#pragma once
#include "Singleton.h"
#include "ICommand.h"
#include <deque>
#include <memory>


class HistoryManager : public Singleton<HistoryManager>
{
public:
    void Do(std::unique_ptr<ICommand> cmd)
    {
        cmd->Execute();

        m_undoStack.push_back(std::move(cmd));

        while (m_undoStack.size() > MAX_HISTORY_COUNT)
        {
            m_undoStack.pop_front();
        }

        m_redoStack.clear();

        m_isDirty = true;
    }

    void Undo()
    {
        if (m_undoStack.empty()) return;

        std::unique_ptr<ICommand> cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();

        cmd->Undo();

        m_redoStack.push_back(std::move(cmd));

        m_isDirty = true;
    }

    void Redo()
    {
        if (m_redoStack.empty()) return;

        std::unique_ptr<ICommand> cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();

        cmd->Execute();

        m_undoStack.push_back(std::move(cmd));

        m_isDirty = true;
    }

    void MarkAsSaved()
    {
        m_isDirty = false;
    }

    bool IsDirty() const
    {
        return m_isDirty;
    }

    void SetDirty()
    {
        m_isDirty = true;
    }

private:
    std::deque<std::unique_ptr<ICommand>> m_undoStack;
    std::deque<std::unique_ptr<ICommand>> m_redoStack;
    static constexpr int MAX_HISTORY_COUNT = 100;

    bool m_isDirty = false;
};