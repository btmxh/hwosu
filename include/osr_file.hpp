#pragma once
#include "io_utils.hpp"

namespace hwo {
    enum class GameMode {
        STANDARD = 0, TAIKO, CTB, MANIA
    };

    struct ReplayFrame {
        io::binary_long timeOffset;
        io::binary_float x, y;
        io::binary_int keys;
    };

    struct ReplayData {
        std::vector<ReplayFrame> frames;
    };

    struct OsrFile {
        GameMode mode;
        io::binary_int version;
        std::string mapMd5, playerName, replayMd5;
        io::binary_short no_300s, no_100s, no_50s, no_gekis, no_katus, no_misses;
        io::binary_int score;
        io::binary_short maxCombo;
        io::binary_byte fullCombo;
        io::binary_int mods;
        std::string lifeBar;
        io::binary_long timestamp;
        ReplayData replayData;
        io::binary_long scoreID;
        io::binary_double additionalModInfo;

        OsrFile(const fs::path& path);
    };
}