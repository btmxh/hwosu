#pragma once
#include "io_utils.hpp"
#include "gl_utils.hpp"

#include <type_traits>
#include <memory>

namespace hwo {
    class Skin {
    public:
        Skin();
        Skin(const fs::path& path);

        template<typename T = fs::path, typename _Str, class = std::is_convertible<const fs::path&, T>>
        std::unique_ptr<T> GetResource(_Str filename) {
            return std::make_unique<T>(m_path / filename);
        }

        gl::TexHandle GetCursor() const {
            return m_cursor->GetHandle();
        }
    private:
        fs::path m_path;

        std::unique_ptr<gl::Texture> m_cursor;
    };
}