#include "skin.hpp"

hwo::Skin::Skin() {}
hwo::Skin::Skin(const fs::path& path): m_path(path), m_cursor(GetResource<hwo::gl::Texture>("cursor.png")) {
}
