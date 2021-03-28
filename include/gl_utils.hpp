#pragma once

#include "glad.h"
#include "io_utils.hpp"

namespace hwo {
    namespace gl {

        //We are using bindless textures
        using TexHandle = GLuint64;

        class Texture {
        public:
            Texture();
            Texture(const fs::path& path);
            ~Texture();

            void MakeResident() const noexcept;
            void MakeNonResident() const noexcept;
            TexHandle GetHandle() const noexcept;
        private:
            GLuint m_tex;
            TexHandle m_handle;
        };

        struct Quad {
            float x1, y1, x2, y2;
            float s1, t1, s2, t2;
            TexHandle texture;
        };

        class Renderer {
        public:
            static constexpr int MAX_QUADS = 256;
            static const float PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT;

            // each quad have 6 vertices (no index buffer), each vertex have 2 floats for position 
            // and 2 floats for texture coordinates
            static constexpr int QUAD_SIZE = sizeof(GLfloat) * 4 * 6;

            static constexpr int TEXTURE_SIZE = sizeof(GLuint64);
            Renderer();
            ~Renderer();

            void begin() noexcept;
            void end() noexcept;

            void quad(const Quad& q) noexcept;
        private:
            GLuint m_vao, m_vbo, m_ubo, m_shader;
            int m_quads;
            float* m_vboData;
            TexHandle* m_uboData;
        };
    }
}
