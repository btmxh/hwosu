#include "osu_file.hpp"

void hwo::OsuFile::SetProperty(const std::string& section, const std::string& key, const std::string& value) {
    size_t line_index = GetPropertyLineIndex(section, key);
    if(line_index == npos) {
        size_t section_line_index = GetSectionLineIndex(section);
        AddLine(key + ":" + value, section_line_index + 1);
    } else {
        SetLine(key + ":" + value, line_index);
    }
}

std::optional<std::string> hwo::OsuFile::GetProperty(const std::string& section, const std::string& key) const {
    size_t line_index = GetPropertyLineIndex(section, key);
    if(line_index == npos)  return {};
    const auto& line = GetLine(line_index);
    return Trim(Trim(Trim(line).substr(key.size())).substr(1));
}

void hwo::OsuFile::GetCommaSeparatedValues(const std::string& section, std::vector<std::string>& values) const noexcept {
    size_t section_index = GetSectionLineIndex(section);
    for(size_t line_index = section_index + 1; line_index < GetLineCount(); line_index++) {
        const auto& line = GetLine(line_index);
        if(IsSectionLine(line)) return;
        values.push_back(line);
    }
}

bool hwo::OsuFile::IsSectionLine(const std::string& line) noexcept {
    std::string trimmed = Trim(line);
    return trimmed.size() >= 2 && trimmed[0] == '[' && trimmed[trimmed.size()] == ']';
}

bool hwo::OsuFile::IsSectionLine(const std::string& line, const std::string& section) noexcept {
    using namespace std::literals::string_literals;
    return Trim(line) == "["s + section + ']';
}

bool hwo::OsuFile::IsPropertyLine(const std::string& line, const std::string& key) noexcept {
    std::string trimmed = Trim(line);
    if(trimmed.find(key, 0) != 0)  return false;
    trimmed = Trim(trimmed.substr(key.size()));
    return trimmed[0] == ':';
}

size_t hwo::OsuFile::GetSectionLineIndex(const std::string& section) const noexcept {
    return GetLineIndex([&](auto& l) { return IsSectionLine(l, section); }, 0, [](auto& l) { return false; });
}

size_t hwo::OsuFile::GetPropertyLineIndex(const std::string& section, const std::string& key) const noexcept {
    size_t section_index = GetSectionLineIndex(section);
    if(section_index == npos)   return npos;
    return GetLineIndex([&](auto& l) { return IsPropertyLine(l, key); }, section_index + 1, [&](auto& l) { return IsSectionLine(l); });
}