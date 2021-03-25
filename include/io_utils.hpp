#pragma once
#include <string_view>
#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>

namespace hwo {
    using integer = int64_t;
    using decimal = long double;
    
    template<typename T>
    static std::optional<T> from_string(const std::string& str);

    template<>
    static std::optional<integer> from_string(const std::string& str) {
        try {
            return std::stoi(str);
        } catch (...) {
            return {};
        }
    }

    template<>
    static std::optional<decimal> from_string(const std::string& str) {
        try {
            return std::stold(str);
        } catch (...) {
            return {};
        }
    }

    namespace fs = std::filesystem;
    namespace io {

        using binary_byte = int8_t;
        using binary_short = int16_t;
        using binary_int = int32_t;
        using binary_long = int64_t;
        using binary_float = float;
        using binary_double = long double;

        class binary_uleb128 {
        public:
            binary_uleb128(uint8_t* iterator);
            binary_uleb128(binary_long value);
            binary_uleb128(const std::string& string);
            binary_uleb128(std::istream& stream);

            operator binary_long() const;
            operator std::string() const;
            size_t size() const;
        private:
            std::vector<uint8_t> m_data;
        };

        class TextFile {
        public:
            static const size_t npos = std::string::npos;

            TextFile(const fs::path& path);

            size_t AddLine(const std::string& line, size_t index = npos) noexcept;
            void SetLine(const std::string& line, size_t index) noexcept;
            size_t GetLineCount() const noexcept;

            const std::string& GetLine(size_t index) const noexcept;
            std::string& GetLine(size_t index) noexcept;

            template<typename _Pred, typename _Pred2>
            size_t GetLineIndex(_Pred linePredicate, size_t offset, _Pred2 breakPredicate) const noexcept {
                for(size_t i = offset; i < m_lines.size(); i++) {
                    if(linePredicate(m_lines[i])) return i;
                    if(breakPredicate(m_lines[i]))    break;
                }
                return npos;
            }
        protected:
            std::vector<std::string> m_lines;
        };

        class BinaryStream {
        public:
            BinaryStream(const fs::path& path);

            void Read(char* data, size_t bytes);

            template<typename T>
            T Read() {
                T value;
                char* value_ptr = reinterpret_cast<char*>(&value);

                Read(value_ptr, sizeof(value));
                if(!m_systemIsLittleEndian) {
                    std::reverse(value_ptr, value_ptr + sizeof(value));
                }

                return value;
            }

            template<>
            binary_uleb128 Read() {
                return ReadULEB128();
            }

            template<>
            std::string Read() {
                return ReadString();
            }
        private:
            bool m_systemIsLittleEndian;
            std::ifstream m_stream;

            binary_uleb128 ReadULEB128();
            std::string ReadString();
        };
    }
}