#include "Arena.h"
#include <windows.h>
typedef struct Renderer_s Renderer;


Renderer* createRenderer(Arena*);
int updateRenderer(Arena*, Renderer*);
