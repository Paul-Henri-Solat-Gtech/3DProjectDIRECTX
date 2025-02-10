#pragma once

// Fenetre
#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <string>

// Debug
#include <cassert> 

// DirectX12
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

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

	virtual void Update();
	virtual void Draw();


private:
	static WindowDX* m_Application; // ref instance de la classe

	HINSTANCE m_appInstance = nullptr; // application instance handle

	// Window parameters
	HWND m_mainWindow = nullptr;
	std::wstring m_windowTitle = L"WINDOW 3D";
	int m_clientWidth = 800;
	int m_clientHeight = 600;

protected:
	// Nouvelle methode pour initialiser DirectX12
	bool InitDirect3D();

	// Membres DirectX12
	UINT mFrameIndex = 0;
	UINT64 mFenceValue = 0;
	ComPtr<ID3D12Fence> mFence;
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	ComPtr<ID3D12Device>           mD3DDevice;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	ComPtr<ID3D12CommandQueue>     mCommandQueue;
	ComPtr<IDXGISwapChain3>        mSwapChain;
	ComPtr<ID3D12DescriptorHeap>   mRtvHeap;
	static const UINT			   mFrameCount = 2; // Double buffering
	ComPtr<ID3D12Resource>         mRenderTargets[mFrameCount];
	UINT                           mRtvDescriptorSize = 0;

};

