#pragma once

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <d3dx9.h>
#include <string>

namespace D3DXEngine
{
	class D3DRender
	{
	private:
		IDirect3D9* d3d9;
		IDirect3DDevice9* d3dDevice; 

	public:
		bool Init(HINSTANCE, int, int, bool, D3DDEVTYPE);
		void Destroy();
		bool Draw(float);
		int EnterMsgLoop();
		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

	int Create(HINSTANCE);
	void Destroy();
	int Run(HINSTANCE);
}