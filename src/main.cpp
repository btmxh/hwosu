#include <iostream>
#include <cassert>
#include "gl2vid.h"
#include "osu_file.hpp"
#include "osr_file.hpp"

int main() {
    g2v_context* ctx = g2v_create_context();
    g2v_free_context(ctx);

    hwo::OsuFile test("magma/magma_normal_diff.osu");
    auto id_optional = test.GetValue<hwo::integer>("Metadata", "BeatmapID");
    std::cout << "cosi" << std::endl;
    assert(id_optional.has_value());
    auto id = id_optional.value();
    std::cout << id << std::endl;

    test.SetProperty("Metadata", "BeatmapID", "727");
    id_optional = test.GetValue<hwo::integer>("Metadata", "BeatmapID");
    assert(id_optional.has_value());
    id = id_optional.value();
    std::cout << id << std::endl;

    uint8_t bytes[3] = { 0xe5, 0x8e, 0x26 };
    hwo::io::binary_uleb128 uleb = 624485L;
    assert(uleb == 624485L);

    hwo::OsrFile osr("magma/wc_replay.osr");
    std::cout << "ffs" << std::endl;
    assert(osr.playerName == "WhiteCat");
    assert(osr.fullCombo == 0);
    assert(osr.scoreID == 2912737366L);
    assert(osr.replaySeed == 12697024);
    std::cout << osr.score << std::endl;

    return 0;
}

