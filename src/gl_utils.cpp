#include "gl_utils.hpp"

#include <cassert>
#include <string_view>
#include <stdexcept>
#ifndef NDEBUG
#include <iostream>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr std::string_view VERTEX_SHADER = R"(
#version 450 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tcoords;

layout (location = 0) out vec2 out_tcoords;
layout (location = 1) flat out int out_quadidx;

layout (location = 0) uniform mat4 ortho;

void main() {
    gl_Position = ortho * vec4(pos, 0.0, 1.0);
    out_tcoords = tcoords;
    out_quadidx = gl_VertexID / 6;
}
)";

constexpr std::string_view FRAGMENT_SHADER = R"(
#version 450 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

#define MAX_QUADS 256

layout (location = 0) in vec2 tcoords;
layout (location = 1) flat in int quadidx;

layout (location = 0) out vec4 color;

layout (std140, binding = 0) uniform TexHandles {
    uint64_t textures[MAX_QUADS];
};

void main() {
    sampler2D tex = sampler2D(textures[quadidx]);
    color = texture(tex, tcoords);
}
)";

const float hwo::gl::Renderer::PLAYFIELD_WIDTH = 480.0f;
const float hwo::gl::Renderer::PLAYFIELD_HEIGHT = 360.0f;

hwo::gl::Texture::Texture(): m_tex(0) {}

hwo::gl::Texture::Texture(const hwo::fs::path& path) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_tex);
    int w, h, c;

    std::string pstr = path.string();    //path.c_str() may return wchar_t*, not supported by stb_image
    stbi_uc* data = stbi_load(pstr.c_str(), &w, &h, &c, STBI_rgb_alpha);
    if(!data) {
        throw std::runtime_error("Error loading image");
    }

    glTextureParameteri(m_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(m_tex, 1, GL_RGBA8, w, h);
    glTextureSubImage2D(m_tex, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

    m_handle = glGetTextureHandleARB(m_tex);
    MakeResident();
}

hwo::gl::Texture::~Texture() {
    if(!m_tex)  return;
    MakeNonResident();
    glDeleteTextures(1, &m_tex);
}

void hwo::gl::Texture::MakeResident() const noexcept {
    glMakeTextureHandleResidentARB(m_handle);
}

void hwo::gl::Texture::MakeNonResident() const noexcept {
    glMakeTextureHandleNonResidentARB(m_handle);
}

hwo::gl::TexHandle hwo::gl::Texture::GetHandle() const noexcept {
    return m_handle;
}

hwo::gl::Renderer::Renderer() {
    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(2, &m_vbo);

    glNamedBufferStorage(m_vbo, QUAD_SIZE * MAX_QUADS, nullptr, GL_MAP_WRITE_BIT);

    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, QUAD_SIZE / 6);
    glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, false, 0);
    glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, false, 2 * sizeof(float));
    glVertexArrayAttribBinding(m_vao, 0, 0);
    glVertexArrayAttribBinding(m_vao, 1, 0);
    glEnableVertexArrayAttrib(m_vao, 0);
    glEnableVertexArrayAttrib(m_vao, 1);

    glNamedBufferStorage(m_ubo, TEXTURE_SIZE * MAX_QUADS, nullptr, GL_MAP_WRITE_BIT);

    m_shader = glCreateProgram();
    auto createShader = [=](GLenum type, const std::string_view& source) {
        auto s = glCreateShader(type);
        const GLchar* s_data = reinterpret_cast<const GLchar*>(source.data());
        GLint s_size = static_cast<GLint>(source.size());
        glShaderSource(s, 1, &s_data, &s_size);
        glCompileShader(s);
#ifndef NDEBUG
        int check;
        glGetShaderiv(s, GL_COMPILE_STATUS, &check);
        if(!check) {
            char log[256];
            glGetShaderInfoLog(s, 256, &check, log);
            std::cout << "Shader: " << log << std::endl;
        }
#endif
        glAttachShader(m_shader, s);
        return s;
    };

    auto vs = createShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    auto fs = createShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);

    glLinkProgram(m_shader);
    glValidateProgram(m_shader);

// Shader in release builds must work
#ifndef NDEBUG
    int check;
    glGetProgramiv(m_shader, GL_LINK_STATUS, &check);
    if(!check) {
        char log[256];
        glGetProgramInfoLog(m_shader, 256, &check, log);
        std::cout << "Shader: " << log << std::endl;
    }
#endif

    glDetachShader(m_shader, vs);
    glDetachShader(m_shader, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(m_shader);
    constexpr float left = 0.0f, right = 640.0f, top = 0.0f, bottom = 480.0f, far = 1.0f, near = -1.0f;

    GLfloat ortho[16] = {};
    ortho[0] = 2.0f / (right - left);
    ortho[5] = 2.0f / (top - bottom);
    ortho[10] = - 2.0f / (far - near);
    ortho[12] = - (right + left) / (right - left);
    ortho[13] = - (top + bottom) / (top - bottom);
    ortho[14] = - (far + near) / (far - near);
    ortho[15] = 1.0f;

    // ortho[0] = 1.0f;
    // ortho[5] = 1.0f;
    // ortho[10] = 1.0f;
    // ortho[15] = 1.0f;

    glUniformMatrix4fv(0, 1, false, ortho);
}

hwo::gl::Renderer::~Renderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(2, &m_vbo);
    glDeleteProgram(m_shader);
}

void hwo::gl::Renderer::begin() noexcept {
    m_vboData = reinterpret_cast<float*>(glMapNamedBuffer(m_vbo, GL_WRITE_ONLY));
    m_uboData = reinterpret_cast<TexHandle*>(glMapNamedBuffer(m_ubo, GL_WRITE_ONLY));
    glUseProgram(m_shader);
    glBindVertexArray(m_vao);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ubo);
    m_quads = 0;
}

void hwo::gl::Renderer::end() noexcept {
    glUnmapNamedBuffer(m_vbo);
    glUnmapNamedBuffer(m_ubo);
    glDrawArrays(GL_TRIANGLES, 0, m_quads * 6);
}

void hwo::gl::Renderer::quad(const Quad& quad) noexcept {
#define PUT(x, y, s, t) *(m_vboData++) = x; *(m_vboData++) = y; *(m_vboData++) = s; *(m_vboData++) = t;
    assert(m_quads <= MAX_QUADS);

    PUT(quad.x1, quad.y1, quad.s1, quad.t1);
    PUT(quad.x2, quad.y1, quad.s2, quad.t1);
    PUT(quad.x2, quad.y2, quad.s2, quad.t2);
    PUT(quad.x2, quad.y2, quad.s2, quad.t2);
    PUT(quad.x1, quad.y2, quad.s1, quad.t2);
    PUT(quad.x1, quad.y1, quad.s1, quad.t1);
    
    *(m_uboData++) = quad.texture;
    m_quads++;
}
