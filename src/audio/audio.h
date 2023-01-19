//
// Created by bison on 05-04-22.
//

#ifndef GAME_AUDIO_H
#define GAME_AUDIO_H

#include <store.h>
#include <SDL_mixer.h>

typedef struct SoundSet SoundSet;
typedef struct SoundFx SoundFx;

struct SoundFx {
    char name[64];
    Mix_Chunk *chunk;
};

struct SoundSet {
    Store soundStore;
    Mix_Music *music;
};

void Audio_SoundSet_Create(SoundSet *set);
void Audio_SoundSet_Destroy(SoundSet *set);
i32 Audio_SoundSet_AddFx(SoundSet *set, const char *name, const char *filename);
bool Audio_SoundSet_LoadMusic(SoundSet *set, const char *filename);
i32 Audio_SoundSet_Lookup(SoundSet *set, const char *name);
void Audio_SoundSet_PlayFx(SoundSet *set, i32 id);
void Audio_SoundSet_PlayMusic(SoundSet *set);

#endif //GAME_AUDIO_H
