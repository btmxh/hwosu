#pragma once

// clang-format off
#include <stdint.h>
#include "x264.h"
// clang-format on

#include <fstream>

/* Simple C++ wrapper for libx264 */

namespace x264 {
    x264_param_t GetDefaultPreset(const char *preset);

    class Encoder {
    public:
        Encoder(x264_param_t& params, const char* preset, const char* profile, const char* filename);
        ~Encoder();

        void SetPlanePointer(void* ptr, int plane) noexcept;
        void EncodeFrame();
    private:
        int m_width, m_height;
        x264_picture_t m_pic;
        x264_picture_t m_pic_out;
        x264_t * m_enc;
        x264_nal_t *m_nal;
        int m_i_nal;
        std::ofstream m_output;
    };
}