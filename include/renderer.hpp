#pragma once

#include "osr_file.hpp"
#include "osu_file.hpp"
#include "timer.hpp"
#include "gl_utils.hpp"
#include "skin.hpp"

#include <memory>

namespace hwo {
    class Renderer {
    public:
        Renderer(int fps);

        void SetReplay(const fs::path& replay_file);
        void SetBeatmap(const fs::path& beatmap_file);
        void SetSkin(const fs::path& skin_path);

        void RenderFrame();
    private:
        std::unique_ptr<OsrFile> m_replay;
        std::unique_ptr<OsuFile> m_beatmap;
        Skin m_skin;
        Timer m_timer;
        gl::Renderer m_gl;
        
        //CACHE
        size_t m_cached_i = 0;

    };
}
