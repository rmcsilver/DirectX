#pragma once

#include <Windows.h>
#include <string>

//D3DRender
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <d3dx9.h>

//Input
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "winmm.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define WIN32_LEAN_AND_MEAN

namespace D3DXEngine
{
	const ULONG ENGINE_MAX_UNFIXSTRING_LENGTH = 65535;
	const ULONG ENGINE_MAX_KEYBOARD_INFO_LENGTH = 256;
	const ULONG ENGINE_MAX_MOUSE_INFO_LENGTH = 3;

	//������ ����-----------------------------------------------------------------------------/
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
	
		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

		int InitWindows(wchar_t*, int&, int&);
		void DestroyWindows();
		int ChangeResolution(int);

		int GetWidth(){return width;}
		int GetHeight(){return height;}
		HINSTANCE GetInstance() {return hInstance;}
		HWND GetHwnd() {return hWnd;}
		bool GetWindoMode() {return !bFullScreen;}
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

	//������ ����-----------------------------------------------------------------------------/


	//Input ����-----------------------------------------------------------------------------/
	enum eInputType 
	{ 
		USE_NONE, 
		USE_KEYBOARD, 
		USE_MOUSE 
	};

	enum eInputState 
	{ 
		eInput_None, 
		eInput_Press, 
		eInput_Release, 
		eInput_Pressing 
	};

	struct InputInfo
	{
		DWORD  dwPressedTime;   // Ű�� ���� �ð� -> Pressed Time.
		DWORD  dwPressingTime;   // Ű�� �������ִ� �ð�. -> Pressing Time.
		eInputState State;     // ���� Ű�� ����.
	};

	class Input
	{
	public:
		int Init(Window&);
		void Destroy();
		bool Process();

		InputInfo* const GetKeyboardInfo(){return stKeyboardInfo;}
		InputInfo* const GetMouseInfo(){return stMouseInfo;}

	private:
		bool Init(HWND, HINSTANCE, int, int);
		bool ReadKeyboard();
		bool ReadMouse();
		void ProcessInput();

		//Ű���� ó��
		bool IsKeyPressing(INT key) const;
		bool IsKeyPress(INT key) const;
		bool IsKeyRelease(INT key) const;
		DWORD GetKeyPressingTime(INT key) const;
		bool IsEscapePressed();

		//���콺 ó��
		void GetMouseLocation(int&, int&);

		bool IsMousePressing(INT mouse) const;
		bool IsMousePress(INT mouse) const;
		bool IsMouseRelease(INT mouse) const;
		int IsMouseMovedX() const;
		int IsMouseMovedY() const;
		DWORD GetMousePressingTime(INT mouse) const;
		
		void GetMouseWindowPosition(POINT* pt) const;	
		void GetMouseMonitorPosition(POINT* pt) const;
		int GetMouseWheelMoved() const;

	private:
		HWND hWnd;
		IDirectInput8* directInput;
		IDirectInputDevice8* keyboard;
		IDirectInputDevice8* mouse;

		//Ű���� ����
		InputInfo stKeyboardInfo[ENGINE_MAX_KEYBOARD_INFO_LENGTH];
		BYTE byteKeyboardState[ENGINE_MAX_KEYBOARD_INFO_LENGTH];
		BYTE byteOldkeyboardState[ENGINE_MAX_KEYBOARD_INFO_LENGTH];

		//���콺 ����
		InputInfo stMouseInfo[ENGINE_MAX_MOUSE_INFO_LENGTH];
		BYTE byteMouseState[ENGINE_MAX_MOUSE_INFO_LENGTH];
		BYTE byteOldMouseState[ENGINE_MAX_MOUSE_INFO_LENGTH];
		DIMOUSESTATE m_diMouseState;
		int mouseMovedX;      // ���콺�� X������ ������ ����.
		int mouseMovedY;      // ���콺�� Y������ ������ ����.
		int mouseWheel;      // ���콺 ���� ������ ����.

		int screenWidth, screenHeight;
		int mouseX, mouseY;

	};
	//Input ����-----------------------------------------------------------------------------/


	//������ ����-----------------------------------------------------------------------------/
	class D3DRender
	{
	private:
		IDirect3D9* d3d9;
		IDirect3DDevice9* d3dDevice; 
	
		//�ﰢ��
		IDirect3DVertexBuffer9* Triangle;

		//ť��
		IDirect3DVertexBuffer9* VB;
		IDirect3DIndexBuffer9*  IB;

		int width;
		int height;

	public:
		D3DRender();
		bool Init(Window&);
		void Destroy();
		bool Draw(float);

		bool CreateTriangle();
		bool DrawTriangle();

		bool CreateCube();
		bool DrawCube(float);

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
	//������ ����-----------------------------------------------------------------------------/

	
	int Create();
	void Destroy();

	bool Setup();
	bool Display(float);

	int EnterMsgLoop();
	int Run();
}