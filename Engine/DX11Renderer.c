#include "Renderer.h"
#include <guiddef.h>
#include <windows.h>
#include <d3d11_4.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h> //Used to compile shaders

#define MAX_WINDOWS 8

struct Window {
	HWND hwnd;
	int Width;
	int Height;
	IDXGISwapChain1* swapchain;
	ID3D11RenderTargetView* backbuffer;
	ID3D11DepthStencilView* depthstencilview;
	ID3D11Texture2D* depthstencilbuffer;
};

typedef struct {
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;
	D3D_FEATURE_LEVEL featurelevel;
	//TODO: Look further into different versions of DXGI and if I want to use newer ones
	IDXGIFactory2* factory;
} DX11;


struct Renderer {
	WNDCLASSEX wc;
	const char* CLASS_NAME;
	HMODULE hInstance;
	int numWindows;
	Window* windows[MAX_WINDOWS];
	DX11 dx11;
};

//TODO: Input Layout how? Also actually implement this
//Look into if you can pass the data directly without an input layout. probably not but worth a check
struct ShaderResource {
	char* filename;
	char* pshaderName;
	char* vshaderName;
	ID3D11PixelShader* ps;
	ID3D11VertexShader* vs;
	ID3D11InputLayout* il;
};

struct Mesh {
	ID3D11Buffer ib;
	ID3D11Buffer vb;

	UINT stride;
	UINT offset;

	UINT ib_l;
};

struct RenderObject {
	Mesh mesh;
	ShaderResource sr;
};


const static char* DEFAULT_SHADER_FILE = "Engine/Shaders/default_shader.hlsl";
const static char* DEFAULT_VS = "VShader";
const static char* DEFAULT_PS = "PShader";



int InitializeDXD(Renderer* renderer) {
	HRESULT hr;

//TODO: Use the version of this from the windows guide for doing all safely
	const D3D_FEATURE_LEVEL features[] = {D3D_FEATURE_LEVEL_11_0};
	hr = D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			features,
			1,
			D3D11_SDK_VERSION,
			&renderer->dx11.dev,
			&renderer->dx11.featurelevel,
			&renderer->dx11.devcon
			);
	if(!SUCCEEDED(hr)) {
		printf("Failed to create D3D11Device");
		return 0;
	}

	IDXGIDevice2* dxdev;
	hr = renderer->dx11.dev->lpVtbl->QueryInterface(renderer->dx11.dev, &IID_IDXGIDevice2, (void**) &dxdev);
	IDXGIAdapter2* dxada;
	if(!SUCCEEDED(hr)) printf("0x%x\n", hr);
	hr = dxdev->lpVtbl->GetParent(dxdev, &IID_IDXGIAdapter2, (void**) &dxada);
	if(!SUCCEEDED(hr)) printf("0x%x\n", hr);
	hr = dxada->lpVtbl->GetParent(dxada, &IID_IDXGIFactory2, (void**) &renderer->dx11.factory);
	if(!SUCCEEDED(hr)) printf("0x%x\n", hr);

	dxdev->lpVtbl->Release(dxdev);
	dxada->lpVtbl->Release(dxada);

	return SUCCEEDED(hr);

}

void rendererCleanup(Renderer* renderer) {
	//TODO: Finish this function once the renderer is more fleshed out
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Renderer* createRenderer(Arena* arena) {
	printf("Initializing DX11Renderer...\n");

	Renderer* renderer = PushStructZero(arena, Renderer);

	renderer->hInstance = GetModuleHandle(NULL);
	renderer->CLASS_NAME = "CSIM-BLANK";


	renderer->wc.lpfnWndProc = WindowProc;
	renderer->wc.hInstance = renderer->hInstance;
	renderer->wc.lpszClassName = renderer->CLASS_NAME;
	renderer->wc.cbSize = sizeof(WNDCLASSEX);

	HRESULT hr = RegisterClassEx(&renderer->wc);
	if(!SUCCEEDED(hr)) {
		printf("Failed to create wndclass");
		return NULL;
	}

	int success = InitializeDXD(renderer);

	if(success == 0) {
		printf("Failed to initialize DXD");
		return NULL;
	}

	Window* wind = createWindow(arena, renderer, 1024, 680, "CSIM-Blank");

	if(wind == NULL) return NULL;

	printf("DX11Renderer Created");

	return renderer;
}

int updateRenderer(Arena* arena, Renderer* renderer) {
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(msg.message == WM_QUIT) {
		return 0;
	}
	return 1;

}
Window* createWindow(Arena* arena, Renderer* renderer, int width, int height, char* windowName) {
	if(renderer->numWindows >= MAX_WINDOWS) {
		printf("Tried to create too many windows\n");
		return NULL;
	}
	Window* newWindow = PushStruct(arena, Window);

	newWindow->hwnd = CreateWindowEx(
			0, 										//Optional Window Styles
			renderer->CLASS_NAME,	//Window Class
			windowName,        	//Window Name
			WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,  //Window Style

			//Position and size
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,

			NULL,                 //Parent Window
			NULL, 								//Menu
			renderer->hInstance,  //Instance Handle
			NULL									//Additional application data
			);
	if(newWindow->hwnd == NULL) {
		printf("HWND == NULL, Failed to create window");
		return NULL;
	}

	DXGI_SWAP_CHAIN_DESC1 scd;
		scd.Height = 0;
		scd.Width = 0;
		scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.Stereo = FALSE; //3d glasses. Wow
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 2;
		scd.Scaling = DXGI_SCALING_STRETCH;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //This mode is only supported in Windows10+
		scd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
//What do I have to do to use flip model?
//Flip model swapchains have a few additional requirements on top of blt swapchains:

//The buffer count must be at least 2.
//After Present calls, the back buffer needs to explicitly be re-bound to the D3D11 immediate context before it can be used again.
//After calling SetFullscreenState, the app must call ResizeBuffers before Present.
//MSAA swapchains are not directly supported in flip model, so the app will need to do an MSAA resolve before issuing the Present.
	HRESULT hr = renderer->dx11.factory->lpVtbl->CreateSwapChainForHwnd(renderer->dx11.factory, (IUnknown*) renderer->dx11.dev, newWindow->hwnd, &scd, NULL, NULL, &newWindow->swapchain);

	if(!SUCCEEDED(hr)) {
		printf("Failed to create swapchain%x\n", hr);
		return NULL;
	}

	//Get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	newWindow->swapchain->lpVtbl->GetBuffer(newWindow->swapchain, 0, &IID_ID3D11Texture2D, (void**)&pBackBuffer);
	//Use the back buffer address to create the render target
	renderer->dx11.dev->lpVtbl->CreateRenderTargetView(renderer->dx11.dev, (ID3D11Resource*) pBackBuffer, NULL, &newWindow->backbuffer);
	pBackBuffer->lpVtbl->Release(pBackBuffer);

	//Create Depth Stencil
	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	depth_stencil_desc.Width = width;
	depth_stencil_desc.Height = height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;

	renderer->dx11.dev->lpVtbl->CreateTexture2D(renderer->dx11.dev, &depth_stencil_desc, NULL, &newWindow->depthstencilbuffer);
	renderer->dx11.dev->lpVtbl->CreateDepthStencilView(renderer->dx11.dev, (ID3D11Resource*)newWindow->depthstencilbuffer, NULL, &newWindow->depthstencilview);



	renderer->windows[renderer->numWindows] = newWindow;
	renderer->numWindows++;
	ShowWindow(newWindow->hwnd, SW_SHOW);

	printf("New Window Created\n");

	return newWindow;

}

Window* createEmptyWindow(Arena* arena, Renderer* renderer, int width, int height, char* windowName) {
	if(renderer->numWindows >= MAX_WINDOWS) {
		printf("Tried to create too many windows\n");
		return NULL;
	}
	Window* newWindow = PushStruct(arena, Window);

	newWindow->hwnd = CreateWindowEx(
			0, 										//Optional Window Styles
			renderer->CLASS_NAME,	//Window Class
			windowName,        	//Window Name
			WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,  //Window Style

			//Position and size
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,

			NULL,                 //Parent Window
			NULL, 								//Menu
			renderer->hInstance,  //Instance Handle
			NULL									//Additional application data
			);
	if(newWindow->hwnd == NULL) {
		printf("HWND == NULL, Failed to create window");
		return NULL;
	}

	renderer->windows[renderer->numWindows] = newWindow;
	renderer->numWindows++;
	ShowWindow(newWindow->hwnd, SW_SHOW);

	printf("New Window Created\n");

	return newWindow;

}

ShaderResource* createShader(Arena* arena, Renderer* renderer, char* vfilename, char* vshaderName, char* pfilename, char* pshaderName) {

	//If empty, use default parameters
	if(vfilename == NULL /*|| vfilename == ""*/) vfilename = (char*) DEFAULT_SHADER_FILE;
	if(vshaderName == NULL /*|| vshaderName == ""*/) vshaderName = (char*) DEFAULT_VS;
	if(pfilename == NULL /*|| pfilename == ""*/) pfilename = (char*) DEFAULT_SHADER_FILE;
	if(pshaderName == NULL /*|| pshaderName == ""*/) pshaderName = (char*) DEFAULT_PS;

	HRESULT hr;

	ShaderResource* sr = PushStruct(arena, ShaderResource);

	ID3DBlob *VS, *PS;

	hr = D3DCompileFromFile((wchar_t*) vfilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, vshaderName, "vs_4_0", 0, 0, &VS, 0);
	if(!SUCCEEDED(hr)) {
		printf("Failed to load %s from %s\n", vshaderName, vfilename);
		return NULL;
	}
	hr = D3DCompileFromFile((wchar_t*) pfilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pshaderName, "ps_4_0", 0, 0, &PS, 0);
	if(!SUCCEEDED(hr)) {
		printf("Failed to load %s from %s\n", pshaderName, pfilename);
		return NULL;
	}


	hr = renderer->dx11.dev->lpVtbl->CreateVertexShader(renderer->dx11.dev, VS->lpVtbl->GetBufferPointer(VS), VS->lpVtbl->GetBufferSize(VS), NULL, &sr->vs);
	if(!SUCCEEDED(hr)) {
		printf("Failed to create vertex shader\n");
		return NULL;
	}
	hr = renderer->dx11.dev->lpVtbl->CreatePixelShader(renderer->dx11.dev, PS->lpVtbl->GetBufferPointer(PS), PS->lpVtbl->GetBufferSize(PS), NULL, &sr->ps);
	if(!SUCCEEDED(hr)) {
		printf("Failed to create pixel shader\n");
		return NULL;
	}

	return sr;
}

void ReleaseShaderResource(ShaderResource* sr) {
	sr->ps->lpVtbl->Release(sr->ps);
	sr->vs->lpVtbl->Release(sr->vs);
	sr->il->lpVtbl->Release(sr->il);
}
