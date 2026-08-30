#pragma once

#include <deque>
#include <functional>

class DeletionQueue
{
public:
    void pushFunction(std::function<void()>&& function)
    {
        m_deletors.push_back(std::move(function));
    }

    void flush()
    {
        for(auto it = m_deletors.rbegin(); it != m_deletors.rend(); it++)
        {
            (*it)();
        }
        m_deletors.clear();
    }

private:
    std::deque<std::function<void()>> m_deletors;
};