#pragma once

#include <chrono>

namespace Nova {

    class Timer {
    public:
        Timer() : m_LastFrameTime(Clock::now()) {}

        // Call once per frame. Returns delta time in seconds.
        float Tick() {
            auto now = Clock::now();
            std::chrono::duration<float> delta = now - m_LastFrameTime;
            m_LastFrameTime = now;
            m_DeltaTime = delta.count();
            m_TotalTime += m_DeltaTime;
            m_FrameCount++;
            return m_DeltaTime;
        }

        float GetDeltaTime()  const { return m_DeltaTime; }
        float GetTotalTime()  const { return m_TotalTime; }
        float GetFPS()        const { return m_DeltaTime > 0.0f ? 1.0f / m_DeltaTime : 0.0f; }
        uint64_t GetFrameCount() const { return m_FrameCount; }

    private:
        using Clock = std::chrono::high_resolution_clock;
        Clock::time_point m_LastFrameTime;
        float m_DeltaTime = 0.0f;
        float m_TotalTime = 0.0f;
        uint64_t m_FrameCount = 0;
    };

} // namespace Nova
