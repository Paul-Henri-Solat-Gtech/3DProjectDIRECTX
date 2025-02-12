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
//#include "d3dUtil.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

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
	void OnResize();
	void FlushCommandQueue();
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	// Membres DirectX12
	//UINT mFrameIndex = 0;
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT64 mFenceValue = 0;
	ComPtr<ID3D12Fence> mFence;
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	ComPtr<ID3D12Device>           mD3DDevice;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	ComPtr<ID3D12CommandQueue>     mCommandQueue;
	ComPtr<IDXGISwapChain3>        mSwapChain;
	ComPtr<ID3D12DescriptorHeap>   mRtvHeap;
	ComPtr<ID3D12DescriptorHeap>   mDsvHeap;
	static const int			   SwapChainBufferCount = 2; // Double buffering
	int mCurrBackBuffer = 0;
	ComPtr<ID3D12Resource>         mRenderTargets[SwapChainBufferCount];
	UINT                           mRtvDescriptorSize = 0;

	ComPtr<ID3D12Resource> mDepthStencilBuffer;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Set true to use 4X MSAA The default is false.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

};

