#pragma once
#include <chrono>

class Timer
{
public:
    using clock = std::chrono::high_resolution_clock;

    Timer()
    {
        reset();
    }

    void reset()
    {
        m_start = clock::now();
    }

    float getDeltaTime()
    {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - m_start).count();
        m_start = now;
        return std::min(dt, 0.1f); // a cap
    }

private:
    std::chrono::time_point<clock> m_start;
};