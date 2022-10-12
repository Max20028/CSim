#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include <stdio.h>

typedef struct ArenaStruct Arena;



Arena* CreateArena();
void ReleaseArena(Arena*);

void* ArenaPush(Arena* arena, size_t size);
void* ArenaPushZero(Arena* arena, size_t size);

#define PushArray(arena, type, count) (type *) ArenaPush(arena, sizeof(type)*(count))
#define PushArrayZero(arena, type, count) (type *) ArenaPushZero(arena, sizeof(type)*(count))
#define PushStruct(arena, type) PushArray(arena, type, 1)
#define PushStructZero(arena, type) PushArrayZero(arena, type, 1)

void ArenaPop(Arena*, size_t size);

size_t ArenaGetPos(Arena*);

void ArenaSetPosBack(Arena*, size_t size);
void ArenaClear(Arena*);
#endif
