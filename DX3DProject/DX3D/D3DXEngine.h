#pragma once

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <d3dx9.h>
#include <string>

#define WIN32_LEAN_AND_MEAN

namespace D3DXEngine
{
	//렌더러 선언-----------------------------------------------------------------------------/
	class D3DRender
	{
	private:
		IDirect3D9* d3d9;
		IDirect3DDevice9* d3dDevice; 

	public:
		bool Init(HWND, int, int, bool, D3DDEVTYPE);
		void Destroy();
		bool Draw(float);
		int EnterMsgLoop();

		template<class T>void Release(T t)
		{
			if(t)
			{
				t->Release();
				t = 0;
			}
		}

		template<class T>void Delete(T t)
		{
			if(t)
			{
				delete t;
				t = 0;
			}
		}
	};
	//렌더러 선언-----------------------------------------------------------------------------/

	//윈도우 선언-----------------------------------------------------------------------------/
	class Window
	{
	public:
		enum ERESOLUTION
		{
			ERES_800X600,
			ERES_1024X768,
			ERES_MAX,
		};

	public:
		Window();
		int Init();
		void Destroy();
		int PopMessage(MSG&);
	
		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

		int InitWindows(wchar_t*, int&, int&);
		void DestroyWindows();
		int ChangeResolution(int);

		int GetWidth(){return width;}
		int GetHeight(){return height;}
		HINSTANCE GetInstance() {return hInstance;}
		HWND GetHwnd() {return hWnd;}
	private:
		HINSTANCE hInstance;
		HWND hWnd;
		wchar_t* pWcApplicationName;

		int width;
		int height;
		bool bShowCursor;

		ERESOLUTION eResolution;
		bool bFullScreen;
		int widths[256];
		int heights[256];
		int colors[256];
		int modeCount;
		DEVMODE devOriginalMode;

	};

	static Window* WindowHandle = 0;
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	//윈도우 선언-----------------------------------------------------------------------------/

	int Create();
	void Destroy();
	int Run();
}