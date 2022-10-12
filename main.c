#include <windows.h>
#include <stdio.h>
#include <initguid.h>
#include "Engine/Renderer.h"

void testArena();

HRESULT hr;


int main(int argc, const char** argv) {
	printf("Hello World\n");

	Arena* permArena = CreateArena();

	Renderer* renderer = createRenderer(permArena);

	while(updateRenderer(permArena, renderer) > 0) {
		//game
	}
	return 1;
}

void testArena() {
	Arena* arena1 = CreateArena();
	int* a = (int*) ArenaPush(arena1, sizeof(int));
	int* b = ArenaPush(arena1, sizeof(int));

	*a = 10;
	*b = 20;
	*a = 999;

	ArenaPop(arena1, sizeof(int));
	printf("a: %d, b: %d\n", *a, *b);
}
