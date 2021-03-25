#pragma once
#include "io_utils.hpp"
#include "string_utils.hpp"

namespace hwo {
    class OsuFile : public io::TextFile {
    public:
        OsuFile(const fs::path& path) : TextFile(path), m_path(path) {};

        std::optional<std::string> GetProperty(const std::string& section, const std::string& key) const;
        void SetProperty(const std::string& section, const std::string& key, const std::string& value);

        template<typename T>
        std::optional<T> GetValue(const std::string& section, const std::string& key) {
            auto property = GetProperty(section, key);
            if(property) {
                return ::hwo::from_string<T>(property.value());
            } else {
                return {};
            }
        }

        void GetCommaSeparatedValues(const std::string& section, std::vector<std::string>& values) const noexcept;

    private:
        fs::path m_path;

        static bool IsSectionLine(const std::string& line) noexcept;
        static bool IsSectionLine(const std::string& line, const std::string& section) noexcept;
        static bool IsPropertyLine(const std::string& line, const std::string& key) noexcept;
        size_t GetSectionLineIndex(const std::string& section) const noexcept;
        size_t GetPropertyLineIndex(const std::string& section, const std::string& key) const noexcept;
    };
}