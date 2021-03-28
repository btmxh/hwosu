#include <iostream>
#include <cassert>
#include "glad.h"
#include "g2v.hpp"
#include "g2v_cvffmpeg.hpp"
#include "osu_file.hpp"
#include "osr_file.hpp"
#include "gl_utils.hpp"
#include "renderer.hpp"

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param) {
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

constexpr int FPS = 120, SECONDS = 60;
int main() {
	try {
		g2v::Init();

		{
			g2v::RenderContext ctx(1920, 1080);
			g2v::CVFFmpegEncoder enc(ctx, "output.mkv", 120.0, 1000000);

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(message_callback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

			//NVIDIA will throw this dumb warning at us every frame, so it's smart to suppress it. And yes, this is because we don't do multithreading stuff, and I won't do it because no.
			GLuint id = 131154;
			glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, 1, &id, GL_FALSE);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			hwo::Renderer renderer(FPS);
			renderer.SetBeatmap("res/magma/magma_top_diff.osu");
			renderer.SetReplay("res/magma/wc_replay.osr");
			renderer.SetSkin("res/skin");

			enc.SetVideoFrameCallback([&]() {
				renderer.RenderFrame();
				return ctx.GetFrameNumber() > SECONDS * FPS;
			});

			auto start = std::chrono::high_resolution_clock::now();
			enc.Encode();
			auto end = std::chrono::high_resolution_clock::now();
			std::cout << "Time ellapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0f << "s" << std::endl;
		}

		g2v::Terminate();
	} catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

    return 0;
}

