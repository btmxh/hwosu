#include "osr_file.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <memory>
#include "LzmaDec.h"

void *SzAlloc(ISzAllocPtr p, size_t size) { p = p; return malloc(size); }
void SzFree(ISzAllocPtr p, void *address) { p = p; free(address); }
ISzAlloc alloc = { SzAlloc, SzFree };

std::unique_ptr<uint8_t[]> decompressLZMA(uint8_t* src, size_t src_size, size_t& dst_size) {
    if(src_size < 13) throw std::runtime_error("invalid LZMA header");

    UInt64 size = 0;
    for (int i = 0; i < 8; i++) {
        size |= (src[5 + i] << (i * 8));
    }

    auto dst = std::make_unique<uint8_t[]>(size);

    ELzmaStatus lzmaStatus;
    SizeT procOutSize = size, procInSize = src_size - 13;
    auto status = LzmaDecode(dst.get(), &procOutSize, &src[13], &procInSize, src, 5, LZMA_FINISH_END, &lzmaStatus, &alloc);

    if(status == SZ_OK && procOutSize == size) {
        dst_size = size;
        return dst;
    }

    throw std::runtime_error("An error occurred while decompressing LZMA data");
}

hwo::OsrFile::OsrFile(const fs::path& path) {
    using namespace io;
    BinaryStream stream(path);
    mode = static_cast<GameMode>(stream.Read<binary_byte>());
#define READ(x) x = stream.Read<decltype(x)>()
    READ(version);
    READ(mapMd5);
    READ(playerName);
    READ(replayMd5);
    READ(no_300s);
    READ(no_100s);
    READ(no_50s);
    READ(no_gekis);
    READ(no_katus);
    READ(no_misses);
    READ(score);
    READ(maxCombo);
    READ(fullCombo);
    READ(mods);
    READ(lifeBar);
    READ(timestamp);
    
    size_t compressSize = stream.Read<binary_int>();
    //LZMA
    std::vector<char> compressData;
    compressData.resize(compressSize);
    stream.Read(compressData.data(), compressSize);
    size_t decompressSize;
    auto decompressData = decompressLZMA(reinterpret_cast<uint8_t*>(compressData.data()), compressSize, decompressSize);
    std::cout << decompressData.get() << std::endl;

    READ(scoreID);
    std::cout << std::hex << "0x" << scoreID << std::dec << std::endl;
    if(mods & (1 << 23)) READ(additionalModInfo); else additionalModInfo = 0;

}