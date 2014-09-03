#include "D3DXEngine.h"

namespace D3DXEngine
{
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

	//Input 정의-------------------------------------------------------------------------/
	bool Input::Init(HWND hwnd, HINSTANCE hinstance, int screenWidth, int screenHeight)
	{
		HRESULT result = 0;

		hWnd = hwnd;

		// Store the screen size which will be used for positioning the mouse cursor.
		screenWidth = screenWidth;
		screenHeight = screenHeight;

		// Initialize the location of the mouse on the screen.
		mouseX = 0;
		mouseY = 0;

		// Initialize the main direct input interface.
		result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);

		// Initialize the direct input interface for the keyboard.
		result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);

		// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
		result = keyboard->SetDataFormat(&c_dfDIKeyboard);

		// Set the cooperative level of the keyboard to share with other programs.
		result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		// Initialize the direct input interface for the mouse.
		result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);

		// Set the data format for the mouse using the pre-defined mouse data format.
		result = mouse->SetDataFormat(&c_dfDIMouse);

		// Set the cooperative level of the mouse to share with other programs.
		result = mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		// Now acquire the keyboard.
		result = keyboard->Acquire();

		// Acquire the mouse.
		result = mouse->Acquire();

		if(FAILED(result))
		{
			return false;
		}

		return true;
	}

	int  Input::Init(Window &window)
	{
		HWND hWnd = window.GetHwnd();
		HINSTANCE hInstance = window.GetInstance();
		int nScreenWidth = window.GetWidth();
		int nScreenHeight = window.GetHeight();
	
		return Init(hWnd, hInstance, nScreenWidth, nScreenHeight);
	}

	void Input::Destroy()
	{
		// Release the mouse.
		if(mouse)
		{
			mouse->Unacquire();
			mouse->Release();
			mouse = 0;
		}

		// Release the keyboard.
		if(keyboard)
		{
			keyboard->Unacquire();
			keyboard->Release();
			keyboard = 0;
		}

		// Release the main interface to direct input.
		if(directInput)
		{
			directInput->Release();
			directInput = 0;
		}

		return;
	}


	bool Input::Process()
	{
		bool result;


		// Read the current state of the keyboard.
		result = ReadKeyboard();
		if(!result)
		{
			return false;
		}

		// Read the current state of the mouse.
		result = ReadMouse();
		if(!result)
		{
			return false;
		}

		// Process the changes in the mouse and keyboard.
		ProcessInput();

		return true;
	}


	bool Input::ReadKeyboard()
	{
		HRESULT result;

		if(keyboard == NULL)
		{
			result = keyboard->Acquire();
			if(FAILED(result))
			{
				return false;
			}
		}

		// Read the keyboard device.
		result = keyboard->GetDeviceState(sizeof(byteKeyboardState), (LPVOID)&byteKeyboardState);
		if(FAILED(result))
		{
			// If the keyboard lost focus or was not acquired then try to get control back.
			if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			{
				keyboard->Acquire();
			}
			else
			{
				return false;
			}
		}

		return true;
	}


	bool Input::ReadMouse()
	{
		HRESULT result;

		if(mouse == NULL)
		{
			result = mouse->Acquire();
			if(FAILED(result))
			{
				return false;
			}
		}

		// Read the mouse device.
		result = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_diMouseState);
		if(FAILED(result))
		{
			// If the mouse lost focus or was not acquired then try to get control back.
			if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			{
				mouse->Acquire();
			}
			else
			{
				return false;
			}
		}

		mouseMovedX = (INT)(m_diMouseState.lX);
		mouseMovedY = (INT)(m_diMouseState.lY);
		mouseWheel = (INT)(m_diMouseState.lZ);
		ZeroMemory(&byteMouseState, sizeof(byteMouseState));
		memcpy(byteMouseState, m_diMouseState.rgbButtons, sizeof(byteMouseState));

		// Ensure the mouse location doesn't exceed the screen width or height.
		if(mouseX < 0)  { mouseX = 0; }
		if(mouseY < 0)  { mouseY = 0; }

		if(mouseX > screenWidth)  { mouseX = screenWidth; }
		if(mouseY > screenHeight) { mouseY = screenHeight; }

		return true;
	}


	void Input::ProcessInput()
	{
		//////////////////////////////////////////////////////////////////////////
		DWORD dwTime = ::timeGetTime();
		// 상태정보를 구조체에 저장한다.
		for(INT i = 0; i < ENGINE_MAX_KEYBOARD_INFO_LENGTH; i++)
		{
			if(byteOldkeyboardState[i] == 0 && byteKeyboardState[i] == 0)    stKeyboardInfo[i].State = eInput_None;
			else if(byteOldkeyboardState[i] == 0 && byteKeyboardState[i] == 0x80)  stKeyboardInfo[i].State = eInput_Press;
			else if(byteOldkeyboardState[i] == 0x80 && byteKeyboardState[i] == 0)  stKeyboardInfo[i].State = eInput_Release;
			else if(byteOldkeyboardState[i] == 0x80 && byteKeyboardState[i] == 0x80) stKeyboardInfo[i].State = eInput_Pressing;
			// 키의 상태가 None일 경우, Pressed Time을 0으로 초기화한다.
			if(stKeyboardInfo[i].State == eInput_None)
			{
				stKeyboardInfo[i].dwPressedTime = 0;
				stKeyboardInfo[i].dwPressingTime = 0;
			}
			// 키의 상태가 Press일 경우, Pressed Time에 현재 시간값을 저장한다.
			if(stKeyboardInfo[i].State == eInput_Press)
				stKeyboardInfo[i].dwPressedTime = dwTime;
			// 키의 상태가 Pressing 일 경우, 현재 시간값에서 Pressed Time을 빼서 Pressing Time을 구한다.
			if(stKeyboardInfo[i].State == eInput_Pressing)
				stKeyboardInfo[i].dwPressingTime = dwTime - stKeyboardInfo[i].dwPressedTime;
		}
		// 현재 키의 상태를 이전 키 상태배열에 복사한다.
		memcpy(byteOldkeyboardState, byteKeyboardState, sizeof(byteKeyboardState));
		//////////////////////////////////////////////////////////////////////////

		// 현재 시간값(윈도우가 부팅한 이후의 카운트 값)을 얻어온다.
		dwTime = ::timeGetTime();
		// 모든 마우스에 대해서, 마우스보드 상태 정보를 구조체에 저장한다.
		for(INT i = 0; i < ENGINE_MAX_MOUSE_INFO_LENGTH; i++)
		{
			if(byteOldMouseState[i] == 0 && byteMouseState[i] == 0)    stMouseInfo[i].State = eInput_None;
			else if(byteOldMouseState[i] == 0 && byteMouseState[i] == 0x80)  stMouseInfo[i].State = eInput_Press;
			else if(byteOldMouseState[i] == 0x80 && byteMouseState[i] == 0)  stMouseInfo[i].State = eInput_Release;
			else if(byteOldMouseState[i] == 0x80 && byteMouseState[i] == 0x80) stMouseInfo[i].State = eInput_Pressing;
			// 마우스의 상태가 None일 경우, Pressed Time을 0으로 초기화한다.
			if(stMouseInfo[i].State == eInput_None)
			{
				stMouseInfo[i].dwPressedTime = 0;
				stMouseInfo[i].dwPressingTime = 0;
			}
			// 마우스의 상태가 Press일 경우, Pressed Time에 현재 시간값을 저장한다.
			if(stMouseInfo[i].State == eInput_Press)
				stMouseInfo[i].dwPressedTime = dwTime;
			// 마우스의 상태가 Pressing 일 경우, 현재 시간값에서 Pressed Time을 빼서 Pressing Time을 구한다.
			if(stMouseInfo[i].State == eInput_Pressing)
				stMouseInfo[i].dwPressingTime = dwTime - stMouseInfo[i].dwPressedTime;
		}
		// 현재 마우스의 상태를 이전 마우스 상태배열에 복사한다.
		memcpy(byteOldMouseState, byteOldMouseState, sizeof(byteOldMouseState));
		//////////////////////////////////////////////////////////////////////////
	
		// Update the location of the mouse cursor based on the change of the mouse location during the frame.
		mouseX += m_diMouseState.lX;
		mouseY += m_diMouseState.lY;

		// Ensure the mouse location doesn't exceed the screen width or height.
		if(mouseX < 0)  { mouseX = 0; }
		if(mouseY < 0)  { mouseY = 0; }

		if(mouseX > screenWidth)  { mouseX = screenWidth; }
		if(mouseY > screenHeight) { mouseY = screenHeight; }

		return;
	}


	bool Input::IsEscapePressed()
	{
		// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
		if(byteKeyboardState[DIK_ESCAPE] & 0x80)
		{
			return true;
		}

		return false;
	}


	void Input::GetMouseLocation(int& mouseX, int& mouseY)
	{
		mouseX = mouseX;
		mouseY = mouseY;
		return;
	}

	bool Input::IsKeyPressing(INT key) const
	{
		if(stKeyboardInfo[key].State == eInput_Pressing)
			return true;
		return false;
	}

	bool Input::IsKeyPress(INT key) const
	{
		if(stKeyboardInfo[key].State == eInput_Press)
			return true;
		return false;
	}

	bool Input::IsKeyRelease(INT key) const
	{
		if(stKeyboardInfo[key].State == eInput_Release)
			return true;
		return false;
	}

	DWORD Input::GetKeyPressingTime(INT key) const
	{
		return stKeyboardInfo[key].dwPressingTime;
	}

	bool Input::IsMousePressing(INT mouse) const
	{
		if(stMouseInfo[mouse].State == eInput_Pressing)
			return true;
		return false;
	}

	bool Input::IsMousePress(INT mouse) const
	{
		if(stMouseInfo[mouse].State == eInput_Press)
			return true;
		return false;
	}

	bool Input::IsMouseRelease(INT mouse) const
	{
		if(stMouseInfo[mouse].State == eInput_Release)
			return true;
		return false;
	}

	DWORD Input::GetMousePressingTime(INT mouse) const
	{
		return stMouseInfo[mouse].dwPressingTime;
	}

	INT  Input::IsMouseMovedX() const
	{
		return mouseMovedX;
	}

	INT  Input::IsMouseMovedY() const
	{
		return mouseMovedY;
	}

	INT Input::GetMouseWheelMoved() const
	{
		return mouseWheel;
	}

	VOID Input::GetMouseMonitorPosition( POINT* pt ) const
	{
		// 모니터(바탕화면)을 기준으로 마우스의 위치를 얻는다.
		GetCursorPos(pt);
	}

	VOID Input::GetMouseWindowPosition( POINT* pt ) const
	{
		// 모니터(바탕화면)을 기준으로 마우스의 위치를 얻는다.
		GetCursorPos(pt);
		// 모니터 영역의 좌표를 클라이언트 영역의 좌표로 변경한다.
		ScreenToClient(hWnd, pt);
	}	
	//Input 정의-------------------------------------------------------------------------/

	//렌더러 정의-----------------------------------------------------------------------------------/

	struct Vertex
	{
	Vertex(){}

	Vertex(float x, float y, float z)
	{
		_x = x;	 _y = y;  _z = z;
	}

	float _x, _y, _z;

	static const DWORD FVF;
	};
	const DWORD Vertex::FVF = D3DFVF_XYZ;

	bool D3DRender::Init(Window &window)
	{
		//D3D 초기화
		d3d9 = NULL;
		d3dDevice = NULL; 
		Triangle = NULL;
		HRESULT hr = 0;
		D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
		HWND hWnd = window.GetHwnd();
		width = window.GetWidth();
		height = window.GetHeight();
		bool bWindowed = window.GetWindoMode();

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
	
	void D3DRender::Destroy()
	{
		Release(d3d9);
		Release(d3dDevice);
	}

	bool D3DRender::Draw()
	{
		if(d3dDevice)
		{
			d3dDevice->Clear(0,0,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3dDevice->BeginScene();

			d3dDevice->SetStreamSource(0,Triangle,0,sizeof(Vertex));
			d3dDevice->SetFVF(Vertex::FVF);

			d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

			d3dDevice->EndScene();
			d3dDevice->Present(0,0,0,0);
		}

		return true;
	}

	bool D3DRender::CreateTriangle()
	{
		d3dDevice->CreateVertexBuffer(3*sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &Triangle, 0);

		Vertex* vertices;
		Triangle->Lock(0,0,(void**)&vertices, 0);

		vertices[0] = Vertex(-1.0f, 0.0f, 2.0f);
		vertices[1] = Vertex(0.0f, 1.0f, 2.0f);
		vertices[2] = Vertex(1.0f, 0.0f, 2.0f);

		Triangle->Unlock();

		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)width/(float)height, 1.0f, 10000.0f);

		d3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

		return true;
	}


	//렌더러 정의-----------------------------------------------------------------------------------/

	Window window;
	D3DRender render;
	Input input;

	int Create()
	{
		window.Init();
		render.Init(window);
		input.Init(window);

		return 1;
	}

	void Destroy()
	{
		input.Destroy();
		render.Destroy();
		window.Destroy();
	}

	int EnterMsgLoop()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		static float lastTime = (float)timeGetTime(); 

		while(msg.message != WM_QUIT)
		{
			if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{	
				float currTime  = (float)timeGetTime();
				float timeDelta = (currTime - lastTime)*0.001f;

				Display(timeDelta);
				input.Process();

				lastTime = currTime;
			}
		}
		return msg.wParam;
	}

	bool Setup()
	{
		render.CreateTriangle();

		return 1;
	}

	bool Display(float timeDelta)
	{
		render.Draw();

		return 1;
	}

	int Run()
	{
		if(!Create())
		{
			::MessageBoxW(0,L"Create() - FAILED",0,0);
			return 0;
		}

		if(!Setup())
		{
			::MessageBox(0, L"Setup() - FAILED", 0, 0);
			return 0;
		}

		EnterMsgLoop();
	
		Destroy();

		return 1;
	}
}