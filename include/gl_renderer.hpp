#pragma once

#include "glad.h"

namespace hwo {
    namespace gl {
        //We are using bindless textures
        using Texture = GLuint64;

        class Renderer {
        public:
            Renderer();

            void begin();
            void end();

            void quad(float x, float y, float w, float h, );
        private:
            GLuint vao, vbo;
            float* bufferData;
        };
    }
}