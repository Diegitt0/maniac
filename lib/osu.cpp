#include <maniac/osu.h>

using namespace osu;

constexpr char keys[] = "asdfjkl;";
constexpr size_t keys_len = sizeof(keys) - 1;

Osu::Osu() : Process("osu!.exe") {
    using namespace signatures;

    auto time_future = std::async([&] {
        return read_memory<uintptr_t>(find_pattern(TIME_SIG) + TIME_SIG_OFFSET);
    });

    auto player_future = std::async([&] {
        return read_memory<uintptr_t>(find_pattern(PLAYER_SIG) + PLAYER_SIG_OFFSET);
    });

    time_address = time_future.get();
    player_pointer = player_future.get();
}

Osu::~Osu() = default;

const char* Osu::get_key_subset(int column_count) {
    assert(column_count > 0 && column_count <= 9);

    static char key_subset[10];
    key_subset[column_count] = '\0';

    const size_t subset_offset = (keys_len / 2) - (column_count / 2);

    for (size_t i = 0; i < column_count; i++) {
        key_subset[i] = keys[subset_offset + i];
    }

    if (column_count % 2) {
        key_subset[column_count / 2] = ' ';
    }

    return key_subset;
}

internal::map_player Osu::get_map_player() {
    static internal::map_player player;

    if (!player) {
        player = internal::map_player(read_memory_safe<uintptr_t>("player", player_pointer));
    }

    return player;
}

void Action::log() const {
#ifdef DEBUG
    debug("action:");
    debug("    time: %d", time);
    debug("    key: %c", key);
    debug("    down: %d", down);
#endif
}
