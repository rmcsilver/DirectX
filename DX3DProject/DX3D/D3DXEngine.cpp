#include "D3DXEngine.h"

namespace D3DXEngine
{
	//렌더러 정의-----------------------------------------------------------------------------------/
	bool D3DRender::Init(HWND hWnd, int width, int height, bool bWindowed, D3DDEVTYPE deviceType)
	{
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
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.Windowed = bWindowed;
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hWnd, vp, &d3dpp, &d3dDevice);

		if(FAILED(hr))
		{
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

			hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hWnd, vp, &d3dpp, &d3dDevice);

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

	//렌더러 정의-----------------------------------------------------------------------------------/

	//윈도우 정의-------------------------------------------------------------------------/
	Window::Window()
	:width(0), height(0), bShowCursor(false), bFullScreen(false)
	{
	}

	int Window::Init()
	{
		width = 0;
		height = 0;
		bShowCursor = true;
		hInstance = NULL;
		hWnd = NULL;
		pWcApplicationName = L"GameEngine Ver.02";
		bFullScreen = false;
		eResolution = ERES_800X600;

		::ZeroMemory(widths, sizeof(widths));
		::ZeroMemory(heights, sizeof(heights));
		::ZeroMemory(colors, sizeof(colors));
		modeCount = 0;

		int nResult = 0;
		nResult = InitWindows(pWcApplicationName, width, height);

		return nResult;
	}

	void Window::Destroy()
	{
		DestroyWindows();

		DEVMODE devMode;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

		if( devMode.dmPelsWidth != devOriginalMode.dmPelsWidth
			||  devMode.dmPelsHeight != devOriginalMode.dmPelsHeight
			||  devMode.dmBitsPerPel != devOriginalMode.dmBitsPerPel
			||  devMode.dmFields != devOriginalMode.dmFields
			||  devMode.dmDisplayFrequency != devOriginalMode.dmDisplayFrequency)
		{
			ChangeDisplaySettings(&devOriginalMode, CDS_FULLSCREEN);
		}
	};

	int Window::PopMessage(MSG &msg)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
			return 0;

		return 1;
	}

	int Window::InitWindows(wchar_t* pwcTitle, int& nWidth, int& nHeight)
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX, posY;
		
		WindowHandle = this;

		hInstance = GetModuleHandle(NULL);

		pWcApplicationName = pwcTitle;

		wc.style         = 0;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm       = wc.hIcon;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = pWcApplicationName;
		wc.cbSize        = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);

		nWidth  = GetSystemMetrics(SM_CXSCREEN);
		nHeight = GetSystemMetrics(SM_CYSCREEN);

		//--------------------------------------------------------------------------------------------------

		 //그래픽 카드가 지원하는 해상도 알아내기
		 int iModeNum = 0;
		 DEVMODE devMode;

		 while( EnumDisplaySettings(NULL, iModeNum++, &devMode))
		 {
			 if(iModeNum > 1 
				&& devMode.dmBitsPerPel == colors[modeCount- 1] 
				&& devMode.dmPelsWidth == widths[modeCount - 1] 
				&& devMode.dmPelsHeight == heights[modeCount - 1]) continue;
			  colors[modeCount] = devMode.dmBitsPerPel;
			  widths[modeCount] = devMode.dmPelsWidth;
			  heights[modeCount] = devMode.dmPelsHeight;
			  modeCount++;
		 }

		//현재의 디스플레이 모드를 저장한다.
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devOriginalMode);

		//--------------------------------------------------------------------------------------------------


		if(bFullScreen)
		{
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth  = (unsigned long)nWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)nHeight;
			dmScreenSettings.dmBitsPerPel = 32;			
			dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			posX = posY = 0;
		}
		else
		{
			switch(eResolution)
			{
			case ERES_800X600:
				nWidth  = 800;
				nHeight = 600;
				break;
			case ERES_1024X768:
				nWidth  = 1024;
				nHeight = 768;
				break;
			default:
				nWidth  = 800;
				nHeight = 600;
				break;
			}

			//화면 중앙에 윈도우 이동시킨다.
			posX = (GetSystemMetrics(SM_CXSCREEN) - nWidth)  / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;
		}

		//윈도우 생성
		hWnd = CreateWindowEx(WS_EX_APPWINDOW, pWcApplicationName, pWcApplicationName, 
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			posX, posY, nWidth, nHeight, NULL, NULL, hInstance, NULL);

		if(NULL == hWnd)
		{
			return 0;
		}

		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		ShowCursor(bShowCursor);

		return 1;
	};

	void Window::DestroyWindows()
	{
		//커서를 복원시킨다.
		if(false == bShowCursor)
		{
			ShowCursor(true);
		}

		//풀스크린 모드에서는 모니터 해상도를 복원시킨다.
		if(bFullScreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		DestroyWindow(hWnd);
		hWnd = NULL;

		UnregisterClass(pWcApplicationName, hInstance);
		hInstance = NULL;
	};

	LRESULT CALLBACK Window::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		switch(umsg)
		{
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}

			case WM_CLOSE:
			{
				PostQuitMessage(0);		
				return 0;
			}

			case WM_KEYDOWN:
			{
				switch (wparam) 
				{
				case 0x31:	//'1' Key
					ChangeResolution(0);
					break;
				case 0x32:	//'2' Key
					ChangeResolution(1);
					break;
				case VK_ESCAPE:
					::DestroyWindow(hwnd);	
					break;
				}
				return 0;
			}
			default:
				break;
		}
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	int Window::ChangeResolution(int nResolution)
	{
		ERESOLUTION Resolution = static_cast<ERESOLUTION>(nResolution);
		if(eResolution == eResolution) return -1;
		if(eResolution < ERES_800X600 || eResolution > ERES_1024X768)	return -2;

		int nWidth = 0;
		int nHeight = 0;
		DWORD dwResult = 0;
		RECT crt = {0,};

		switch(eResolution)
		{
		case ERES_800X600:
			nWidth = 800;
			nHeight = 600;
			break;
		case ERES_1024X768:
			nWidth  = 1024;
			nHeight = 768;
			break;
		default:
			nWidth  = 800;
			nHeight = 600;
			break;
		}

		//지원가능한 해상도를 찾는다.
		for(int i=0; i < modeCount; i++)
		{
			if( widths[i] == nWidth && heights[i] ==  nHeight)
			{
				GetWindowRect(hWnd, &crt);

				crt.left = (GetSystemMetrics(SM_CXSCREEN) - nWidth)  / 2;
				crt.top = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

				MoveWindow(hWnd, crt.left, crt.top, nWidth, nHeight, true);

				width = nWidth;
				height = nHeight;

				eResolution = Resolution;

				return 1;
			}
		}

		return 0;
	};

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return WindowHandle->MessageHandler(hwnd, msg, wParam, lParam);
	}
	//윈도우 정의-------------------------------------------------------------------------/

	Window window;
	D3DRender render;

	int Create()
	{
		window.Init();
		render.Init( window.GetHwnd(), window.GetWidth(), window.GetHeight(),true, D3DDEVTYPE_HAL);
		return 1;
	}

	void Destroy()
	{
		render.Destroy();
		window.Destroy();
	}

	int Run()
	{
		if(!Create())
		{
			::MessageBoxW(0,L"Init() - FAILED",0,0);
			return 0;
		}

		render.EnterMsgLoop();

		Destroy();

		return 1;
	}
}