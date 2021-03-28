#pragma once
#include <chrono>

namespace hwo {
    using time = int64_t;   //milliseconds

    class Timer {
    public:
        Timer(int fps);

        time GetCurrentTime() const noexcept;
        void NextFrame() noexcept;
        void SetFrameNumber(int frame) noexcept;
        void SetSpeedModifier(float speed) noexcept;
    private:
        int m_fps;
        int m_frame;
        float m_speedModifier;
    };
}