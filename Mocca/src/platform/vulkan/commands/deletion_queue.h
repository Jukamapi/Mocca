#pragma once

#include <deque>
#include <functional>

class DeletionQueue
{
public:
    std::deque<std::function<void()>> deletors;

    void pushFunction(std::function<void()>&& function)
    {
        deletors.push_back(std::move(function));
    }

    void flush()
    {
        for(auto it = deletors.rbegin(); it != deletors.rend(); it++)
        {
            (*it)();
        }
        deletors.clear();
    }
};