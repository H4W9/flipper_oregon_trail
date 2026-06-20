#include "save_load.h"
#include <storage/storage.h>
#include <furi.h>
#include <string.h>

#define SAVE_DIR  "/ext/apps_data/oregon_trail"
#define SAVE_PATH "/ext/apps_data/oregon_trail/save.bin"

#define SAVE_MAGIC   0x4F54  // 'OT'
#define SAVE_VERSION 1

typedef struct {
    uint16_t magic;
    uint8_t  version;
    uint8_t  _pad;
    Player   players[MAX_PLAYERS];
    int32_t  num_players;
    Trail    trail;
    int32_t  dead_player_idx;
} SaveData;

bool save_game(const GameState* gs, int dead_player_idx) {
    SaveData data;
    memset(&data, 0, sizeof(data));
    data.magic           = SAVE_MAGIC;
    data.version         = SAVE_VERSION;
    memcpy(data.players, gs->players, sizeof(gs->players));
    data.num_players     = (int32_t)gs->num_players;
    data.trail           = gs->trail;
    data.dead_player_idx = (int32_t)dead_player_idx;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(storage, SAVE_DIR);

    File* f = storage_file_alloc(storage);
    bool ok = storage_file_open(f, SAVE_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    if(ok) {
        uint16_t written = storage_file_write(f, &data, sizeof(data));
        ok = (written == sizeof(data));
        storage_file_close(f);
    }
    storage_file_free(f);
    furi_record_close(RECORD_STORAGE);
    return ok;
}

bool load_game(GameState* gs, int* dead_player_idx) {
    SaveData data;
    memset(&data, 0, sizeof(data));

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* f = storage_file_alloc(storage);
    bool ok = storage_file_open(f, SAVE_PATH, FSAM_READ, FSOM_OPEN_EXISTING);
    if(ok) {
        uint16_t got = storage_file_read(f, &data, sizeof(data));
        ok = (got == sizeof(data));
        storage_file_close(f);
    }
    storage_file_free(f);
    furi_record_close(RECORD_STORAGE);

    if(!ok || data.magic != SAVE_MAGIC || data.version != SAVE_VERSION) return false;

    memcpy(gs->players, data.players, sizeof(gs->players));
    gs->num_players = (int)data.num_players;
    gs->trail       = data.trail;
    gs->screen      = SCREEN_TRAIL;
    if(dead_player_idx) *dead_player_idx = (int)data.dead_player_idx;
    return true;
}

bool save_exists(void) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool exists = storage_file_exists(storage, SAVE_PATH);
    furi_record_close(RECORD_STORAGE);
    return exists;
}

void delete_save(void) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_remove(storage, SAVE_PATH);
    furi_record_close(RECORD_STORAGE);
}
