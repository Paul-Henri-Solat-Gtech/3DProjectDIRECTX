#pragma once

// Fenetre
#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>

// Debug
#include <cassert> 

#include <string>

class WindowDX
{
public:
	WindowDX(HINSTANCE hInstance);
	static WindowDX* GetApp();
	
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // Fonction de callback pour gerer les messages de la fenetre
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool InitWindow(); // Initialise la fenetre

	bool Initialize();

	int Run();

private:
	static WindowDX* m_Application; // ref instance de la classe

	HINSTANCE m_appInstance = nullptr; // application instance handle

	// Window parameters
	HWND m_mainWindow = nullptr;
	std::wstring m_windowTitle = L"WINDOW 3D";
	int m_clientWidth = 800;
	int m_clientHeight = 600;
};

