#include "io_utils.hpp"
#include <fstream>
#include <cassert>
#include <algorithm>
#include <functional>

#ifndef NDEBUG
#include <iostream>
#include <stdio.h>
#include <bitset>
#endif

hwo::io::binary_uleb128::binary_uleb128(uint8_t* iterator) {
    do {
        m_data.push_back(*iterator);
        iterator++;
    } while(*iterator & 0x80);
    m_data.push_back(*iterator);
}

hwo::io::binary_uleb128::binary_uleb128(hwo::io::binary_long value) {
    uint64_t uvalue = value;
    do {
        uint8_t byte = uvalue & 0x7f;
        uvalue >>= 7;
        if(uvalue != 0)  byte |= 0x80;
        m_data.push_back(byte);
    } while(uvalue != 0);
}

//TODO: Make this works with huge numbers
hwo::io::binary_uleb128::binary_uleb128(const std::string& string): binary_uleb128(std::stoll(string)) {}

hwo::io::binary_uleb128::binary_uleb128(std::istream& stream) {
    uint8_t byte;
    do {
        stream.read(reinterpret_cast<char*>(&byte), 1);
        m_data.push_back(byte);
    } while(byte & 0x80);
}

//TODO: Make this works with huge numbers
hwo::io::binary_uleb128::operator hwo::io::binary_long() const {
    assert(!m_data.empty());
    uint64_t value = 0;
    int shift = 0;
    for(size_t i = 0; i < m_data.size(); i++) {
        value |= (m_data[i] & 0x7f) << shift;
        shift += 7;
    }
    return value;
}

//TODO: Make this works with huge numbers
hwo::io::binary_uleb128::operator std::string() const {
    return std::to_string(operator binary_long());
}

hwo::io::TextFile::TextFile(const fs::path& path) {
    std::ifstream stream(path.c_str());
    std::string content((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    
    size_t pos = 0;
    size_t prev = 0;
    while ((pos = content.find('\n', prev)) != std::string::npos)
    {
        m_lines.push_back(content.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    m_lines.push_back(content.substr(prev));
}

size_t hwo::io::TextFile::AddLine(const std::string& line, size_t index) noexcept {
    index = std::min(index, m_lines.size());
    m_lines.insert(m_lines.begin() + index, line);

    assert(std::all_of(line.begin(), line.end(), [](auto c) { return c != '\n'; }));
    return index;
}

void hwo::io::TextFile::SetLine(const std::string& line, size_t index) noexcept {
    m_lines[index] = line;
    assert(std::all_of(line.begin(), line.end(), [](auto c) { return c != '\n'; }));
}

size_t hwo::io::TextFile::GetLineCount() const noexcept {
    return m_lines.size();
}

const std::string& hwo::io::TextFile::GetLine(size_t index) const noexcept {
    return m_lines[index];
}

std::string& hwo::io::TextFile::GetLine(size_t index) noexcept {
    return m_lines[index];
}

hwo::io::BinaryStream::BinaryStream(const fs::path& path): m_stream(path.c_str(), std::ios::binary) {
    //dumb hack to check endianness
    int test = 0x00000001;
    m_systemIsLittleEndian = reinterpret_cast<char*>(&test)[0];
}

void hwo::io::BinaryStream::Read(char* data, size_t size) {
    m_stream.read(data, size);
}

hwo::io::binary_uleb128 hwo::io::BinaryStream::ReadULEB128() {
    return binary_uleb128(m_stream);
}

std::string hwo::io::BinaryStream::ReadString() {
    auto flag = Read<uint8_t>();
    if(flag == 0) {
        return "";
    } else {
        assert(flag == 0x0b);
        size_t length = ReadULEB128();
        std::string value(length, '\0');
        Read(value.data(), length);
        return value;
    }
}
