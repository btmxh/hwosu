#include "renderer.hpp"

#include <tuple>
#include <cmath>

hwo::Renderer::Renderer(int fps): m_timer(fps) {}

void hwo::Renderer::SetReplay(const fs::path& replay) {
    m_replay = std::make_unique<decltype(m_replay)::element_type>(replay);
    if(m_replay->mods & (1 << 6)) {
        m_timer.SetSpeedModifier(1.5f);     //DT
    } else if(m_replay->mods & (1 << 8)) {
        m_timer.SetSpeedModifier(0.75f);    //HT
    } else {
        m_timer.SetSpeedModifier(1.0f);     //no DT/HT/NC
    }
}

void hwo::Renderer::SetBeatmap(const fs::path& beatmap) {
    m_beatmap = std::make_unique<decltype(m_beatmap)::element_type>(beatmap);
}

void hwo::Renderer::SetSkin(const fs::path& skin) {
    m_skin = Skin(skin);
}

struct vec2 {
    float x, y;
};

float lerp(float t, float min, float max) {
    return min + t * (max - min);
}

vec2 lerp(float t, vec2 min, vec2 max) {
    return { lerp(t, min.x, max.x), lerp(t, min.y, max.y) };
}

vec2 GetInterpolatedCursorPosition(const hwo::Timer& timer, const hwo::OsrFile& replay, size_t& cached_i) {
    auto time = timer.GetCurrentTime();

    const auto& frames = replay.replayData.frames;
    const vec2 CENTER{ hwo::gl::Renderer::PLAYFIELD_WIDTH / 2, hwo::gl::Renderer::PLAYFIELD_HEIGHT / 2 };
    if(frames.empty())  return CENTER;
    if(time < frames[0].time)   return { frames[0].x, frames[0].y };

    const auto& last = frames[frames.size() - 1];
    if(time > last.time)   return { last.x, last.y };

    /*
    Since time go forwards, keeping the last previous ReplayFrame index and then start looping from that would be a small performance boost
    */
    for(;cached_i < frames.size(); cached_i++) {
        if(frames[cached_i].time > time) {
            auto next = frames[cached_i];
            auto prev = frames[cached_i - 1];
            float t = static_cast<float>(time - prev.time) / (next.time - prev.time);
            return lerp(t, {prev.x, prev.y}, {next.x, next.y});
        }
    }

    return CENTER;
}

void hwo::Renderer::RenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    auto pos = GetInterpolatedCursorPosition(m_timer, *m_replay, m_cached_i);
    m_gl.begin();
    const float size = 32;
    m_gl.quad({pos.x - size, pos.y - size, pos.x + size, pos.y + size, 0.0f, 0.0f, 1.0f, 1.0f, m_skin.GetCursor()});
    m_gl.end();
    // std::cout << pos.x << " " << pos.y << std::endl;
    m_timer.NextFrame();
}
