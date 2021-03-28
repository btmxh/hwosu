#include "timer.hpp"

hwo::Timer::Timer(int fps): m_fps(fps) {}

hwo::time hwo::Timer::GetCurrentTime() const noexcept {
    return hwo::time(m_frame * 1000 * m_speedModifier / m_fps);
}

void hwo::Timer::NextFrame() noexcept {
    m_frame++;
}

void hwo::Timer::SetFrameNumber(int frame) noexcept {
    m_frame = frame;
}

void hwo::Timer::SetSpeedModifier(float speed) noexcept {
    m_speedModifier = speed;
}
