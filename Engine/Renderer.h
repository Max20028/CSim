#include "Arena.h"

//--------------------
//Type Definitions
typedef struct Renderer Renderer;
typedef struct Window Window;
typedef struct ShaderResource ShaderResource;
typedef struct Mesh Mesh;
typedef struct RenderObject RenderObject;
//--------------------

//Call this to create the renderer
Renderer* createRenderer(Arena*);

//Creates a new window. Do not call this outside of the renderer for now as it cannot handle more than one window. I am putting this here to remind me to implement more windows in the future because doing stuff with that would be really cool and I have some ideas
Window* createWindow(Arena*, Renderer*, int width, int height, char* windowName);

//TODO: Implement cleanup code, i.e. release everything
void rendererCleanup(Renderer* renderer);

//-----------------
//Regular calls

//Call this at the start of every frame to clear the view, depth stencil, etc.
void startFrame(Renderer*);

//Call this each frame to prevent windows from thinking the window is hanging/frozen. Also can be used for getting input
int updateRenderer(Arena*, Renderer*);


//Render a single object to the screen. This is the temp version that will need to be expanded with more functionality
int renderObject(Renderer*);

//Presents the rendered frame to the screen
void presentFrame(Renderer*);

//--------------------
//Shaders
//TODO: Input Layout Stuff in ShaderResource

///Create a shader resource from files
///If any of the char* are left NULL is uses default parameters
ShaderResource* createShader(Arena* arena, Renderer* renderer, char* vfilename, char* vshaderName, char* pfilename, char* pshaderName);
//Release shader
void ReleaseShaderResource(ShaderResource* sr);
//--------------------
//Render Objects

//Create a new render object. Still need to figure out how to specify input layout...
RenderObject* createRenderObject(Arena* arena, Renderer* renderer, void* vertexBuffer, int vertexBufferLength, void* indexBuffer, int indexBufferLength);
//Relase the render object
void ReleaseRenderObject(RenderObject*);
