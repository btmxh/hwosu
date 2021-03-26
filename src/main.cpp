#include <iostream>
#include <cassert>
#include "glad.h"
#include "gl2vid.h"
#include "osu_file.hpp"
#include "osr_file.hpp"
#include "gl_utils.hpp"

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return "UNKNOWN";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
        default: return "UNKNOWN";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        default: return "UNKNOWN";
		}
	}();

	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

struct usr {
    hwo::gl::Renderer* r;
    hwo::gl::TexHandle tex;
};

int main() {
    g2v_create_context();

    g2v_render_ctx ctx;
    g2v_encoder enc;

    g2v_init_render_ctx(&ctx, 720, 1280);
    g2v_create_ffmpeg_encoder(&enc, &ctx, 25, "output.mkv");
    // g2v_create_glfw_encoder(&enc, &ctx);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    hwo::gl::Renderer renderer;
    hwo::gl::Texture texture("res/skin/shit meme.png"); //just for testing, add this file if you don't have it
    texture.MakeResident();

    usr u = { &renderer, texture.GetHandle() };
    enc.user_ptr = &u;

    enc.render_video_frame = [](g2v_render_ctx* ctx, void* usrptr) -> int {
        usr* u = reinterpret_cast<usr*>(usrptr);
        glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        u->r->begin();
        hwo::gl::Quad q{160, 120, 480, 360, 0.0f, 0.0f, 1.0f, 1.0f, u->tex };
        u->r->quad(q);
        u->r->end();
        return ctx->current_frame_index >= 20*25;
    };
    std::cout << &enc << std::endl;

    g2v_encode(&enc, &ctx);

    // g2v_finish_glfw_encoder(&enc);
    g2v_finish_ffmpeg_encoder(&enc);
    g2v_free_render_ctx(&ctx);

    g2v_free_context();

    return 0;
}

