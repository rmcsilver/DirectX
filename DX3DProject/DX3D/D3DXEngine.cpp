#include "D3DXEngine.h"

namespace D3DXEngine
{
	bool D3DRender::Init(HINSTANCE hInstance, int width, int height, bool windowed, D3DDEVTYPE deviceType)
	{
		//윈도우 생성
		WNDCLASS wc;

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = L"Direct3D9App";
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = 0;

		if( !RegisterClass(&wc))
		{
			::MessageBoxW(0, L"RegisterClass() - FAILED",0,0);
			return false;
		}

		HWND hwnd = 0;
		hwnd = ::CreateWindowExW(WS_EX_APPWINDOW, wc.lpszClassName, wc.lpszClassName, 
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				0, 0, width, height, 0,0,hInstance, 0);

		if(!hwnd)
		{
			::MessageBoxW(0, L"CreateWindow() - FAILED",0,0);
			return false;
		}

		::ShowWindow(hwnd, SW_SHOW);
		::UpdateWindow(hwnd);

		//D3D 초기화
		HRESULT hr = 0;
		d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		if(!d3d9)
		{
			::MessageBoxW(0, L"Direct3DCreate9() - FAILED",0,0);
			return false;
		}

		D3DCAPS9 caps;
		d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

		int vp = 0;
		if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hwnd;
		d3dpp.Windowed = windowed;
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, &d3dDevice);

		if(FAILED(hr))
		{
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

			hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, &d3dDevice);

			if(FAILED(hr))
			{
				Destroy();
				::MessageBoxW(0, L"CreateDevice() - FAILED",0,0);
				return false;
			}
		}

		return true;
	}

	int D3DRender::EnterMsgLoop()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		static float lastTime = (float)timeGetTime();

		while(msg.message != WM_QUIT)
		{
			if(::PeekMessageA(&msg,0,0,0,PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}
			else
			{
				float currTime = (float)timeGetTime();
				float timeDelta = (currTime - lastTime) * 0.001f;

				Draw(timeDelta);

				lastTime = currTime;
			}
		}

		return msg.wParam;
	}

	void D3DRender::Destroy()
	{
		Release(d3d9);
		Release(d3dDevice);
	}

	bool D3DRender::Draw(float timeDelta)
	{
		if(d3dDevice)
		{
			d3dDevice->Clear(0,0,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
			d3dDevice->Present(0,0,0,0);
		}

		return true;
	}

	LRESULT CALLBACK D3DRender::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch( msg )
		{
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE )
				::DestroyWindow(hwnd);
			break;
		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	D3DRender render;

	int Create(HINSTANCE hInstance)
	{
		return render.Init(hInstance, 1024,768,true, D3DDEVTYPE_HAL);
		

		return 1;
	}

	void Destroy()
	{
		render.Destroy();
	}

	int Run(HINSTANCE hInstance)
	{
		if(!Create(hInstance))
		{
			::MessageBoxW(0,L"Init() - FAILED",0,0);
			return 0;
		}

		render.EnterMsgLoop();

		Destroy();

		return 1;
	}
}