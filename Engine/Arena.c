#include "Arena.h"

//This implementation does not do memory alignment. May want to in future?

struct ArenaStruct {
	void* mem;
	size_t memsize;
	size_t currOffset;
};

#define INITIALSIZE 4095

static Arena* _CreateArena(size_t size) {

	Arena* arena = malloc(sizeof(Arena));

	arena->mem = malloc(size);
	arena->memsize = size;

	arena->currOffset = 0;

	return arena;
}

Arena* CreateArena() {
	return _CreateArena(INITIALSIZE);
}

void ReleaseArena(Arena* arena) {
	free(arena->mem);
	free(arena);
}

void* ArenaPush(Arena* arena, size_t size) {
	if(arena->currOffset + size > arena->memsize) {
		printf("Increasing size of Arena to %d\n", arena->memsize*2);
		arena->mem = realloc(arena->mem, arena->memsize*2);
		arena->memsize = arena->memsize*2;
		assert(arena->mem != NULL && "realloc in Arena returned null. No more space to allocate");
	}
	arena->currOffset += size;

	return arena->mem + arena->currOffset-size;
}

void* ArenaPushZero(Arena* arena, size_t size) {
	void* mem = ArenaPush(arena, size);
	if(mem != NULL)
		memset(arena->mem+arena->currOffset-size, 0, size);
	return mem;
}


void ArenaPop(Arena* arena, size_t size) {
	arena->currOffset -= size;
}

size_t ArenaGetPos(Arena* arena) {
	return arena->currOffset;
}

void ArenaSetPosBack(Arena* arena, size_t size) {
	arena->currOffset = size;
}

void ArenaClear(Arena* arena) {
	arena->currOffset = 0;
}
