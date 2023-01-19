//
// Created by bison on 05-04-22.
//

#include <assert.h>
#include "audio.h"

void Audio_SoundSet_Create(SoundSet *set) {
    memset(set, 0, sizeof(SoundSet));
    STORE_INIT(set->soundStore, sizeof(SoundFx));
    Mix_VolumeMusic(MIX_MAX_VOLUME - (MIX_MAX_VOLUME/4));
}

void Audio_SoundSet_Destroy(SoundSet *set) {
    for(u32 i = 0; i < STORE_SIZE(set->soundStore); ++i) {
        SoundFx *fx = STORE_GET_AT(set->soundStore, i);
        Mix_FreeChunk(fx->chunk);
    }
    if(set->music) {
        Mix_FreeMusic(set->music);
    }
    STORE_DESTROY(set->soundStore);
}

i32 Audio_SoundSet_AddFx(SoundSet *set, const char *name, const char *filename) {
    SoundFx fx;
    memset(&fx, 0, sizeof(SoundFx));
    strcpy(fx.name, name);
    fx.chunk = Mix_LoadWAV(filename);
    if(fx.chunk == NULL) {
        fprintf(stderr, "Unable to load wave file: %s\n", filename);
        return -1;
    }
    STORE_PUSHBACK(set->soundStore, &fx);
    return STORE_SIZE(set->soundStore) - 1;
}

bool Audio_SoundSet_LoadMusic(SoundSet *set, const char *filename) {
    set->music = Mix_LoadMUS(filename);
    if(set->music == NULL) {
        fprintf(stderr, "Unable to load music file: %s\n", filename);
        return false;
    }
    return true;
}

void Audio_SoundSet_PlayFx(SoundSet *set, i32 id) {
    SoundFx *fx = STORE_GET_AT(set->soundStore, (u32) id);
    assert(fx != NULL);
    Mix_PlayChannel(-1, fx->chunk, 0);
}

void Audio_SoundSet_PlayMusic(SoundSet *set) {
    assert(set->music != NULL);
    Mix_PlayMusic(set->music, -1);
}

i32 Audio_SoundSet_Lookup(SoundSet *set, const char *name) {
    for (i32 id = 0; id < STORE_SIZE(set->soundStore); ++id) {
        SoundFx *fx = STORE_GET_AT(set->soundStore, id);
        if(strcmp(name, fx->name) == 0) {
            return id;
        }
    }
    return -1;
}
