#include "Renderer.h"

struct Renderer_s {
	WNDCLASSEX wc;
	const char* CLASS_NAME;
	HWND hwnd;
	HMODULE hInstance;
};

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

	renderer->hwnd = CreateWindowEx(
			0, 										//Optional Window Styles
			renderer->CLASS_NAME,	//Window Class
			"CSIM_BLANK",        	//Window Name
			WS_OVERLAPPEDWINDOW,  //Window Style

			//Position and size
			300, 300, 680, 680,

			NULL,                 //Parent Window
			NULL, 								//Menu
			renderer->hInstance,  //Instance Handle
			NULL									//Additional application data
			);
	if(renderer->hwnd == NULL) {
		printf("HWND == NULL");
		return NULL;
	}

	ShowWindow(renderer->hwnd, SW_SHOW);
	printf("DX11Renderer+Window Created");

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
