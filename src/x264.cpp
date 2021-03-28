#include "x264.hpp"

#include <sstream>
#include <stdexcept>

#define X264(x) err(x, #x)

static int err(int err, const char *fun) {
    if (err >= 0)
        return err;
    std::stringstream ss;
    ss << "x264 error code " << err << " while calling " << fun;
    throw std::runtime_error(ss.str());
}

x264_param_t x264::GetDefaultPreset(const char *preset) {
    x264_param_t params;
    X264(x264_param_default_preset(&params, preset, NULL));
    return params;
}

x264::Encoder::Encoder(x264_param_t &params, const char *preset,
                       const char *profile, const char *filename)
    : m_output(filename, std::ios::binary) {
    X264(x264_param_apply_profile(&params, profile));

    X264(x264_picture_alloc(&m_pic, X264_CSP_I420, params.i_width,
                            params.i_height));
    m_pic.i_pts = 0;

    // We don't want to use x264 allocated memory, but our own pointer (got from
    // glMapNamedBuffer) to avoid copying memory. This is why we don't call
    // x264_picture_clean in destructor, since this memory will be deallocated
    // by GL (and yes x264_picture_clean only free this pointer and memset the
    // picture to 0, so there would be no memory leaks.)
    free(m_pic.img.plane[0]);

    m_enc = x264_encoder_open(&params);
    if (!m_enc) {
        err(-1, "x264_encoder_open(&params)");
    }
}

void x264::Encoder::EncodeFrame() {
    while (x264_encoder_delayed_frames(m_enc)) {
        m_pic.i_pts++;
        int i_frame_size =
            X264(x264_encoder_encode(m_enc, &m_nal, &m_i_nal, &m_pic, &m_pic_out));
        if (i_frame_size) {
            m_output.write(reinterpret_cast<char *>(m_nal->p_payload),
                           i_frame_size);
        }
    }
}

void x264::Encoder::SetPlanePointer(void *ptr, int plane) noexcept {
    m_pic.img.plane[plane] = reinterpret_cast<uint8_t *>(ptr);
}

x264::Encoder::~Encoder() {
    while (x264_encoder_delayed_frames(m_enc)) {
        int i_frame_size = X264(
            x264_encoder_encode(m_enc, &m_nal, &m_i_nal, NULL, &m_pic_out));
        if (i_frame_size) {
            m_output.write(reinterpret_cast<char *>(m_nal->p_payload),
                           i_frame_size);
        }
    }
    x264_encoder_close(m_enc);
}
