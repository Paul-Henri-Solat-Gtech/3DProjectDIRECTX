#include "WindowDX.h"

WindowDX::WindowDX(HINSTANCE hInstance) : m_appInstance(hInstance)
{
    // Only one WindowDX can be constructed
    assert(m_Application == nullptr);
    m_Application = this;
}

WindowDX* WindowDX::m_Application = nullptr;

WindowDX* WindowDX::GetApp()
{
    return m_Application;
}

LRESULT CALLBACK WindowDX::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Verifie si l'application existe avant d'appeler MsgProc
    if (WindowDX::GetApp() == nullptr)
    {
        return DefWindowProc(hwnd, msg, wParam, lParam); // Si l'application n'existe pas, appelle la procedure par défaut
    }

    // Appeler la methode MsgProc sur l'instance active de WindowDX
    return WindowDX::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

bool WindowDX::InitWindow()
{
	// Initialise la classe window
    WNDCLASS windowClass = {};                          // Definition de la structure WNDCLASS
    windowClass.style = CS_HREDRAW | CS_VREDRAW;        // Redessine la fenetre si elle est redimensionnee
    windowClass.lpfnWndProc = MainWndProc;              // Fonction de callback pour gerer les messages
    windowClass.cbClsExtra = 0;                         // Pas d'espace supplementaire alloue pour cette classe
    windowClass.cbWndExtra = 0;                         // Pas d'espace supplementaire alloue pour chaque fenetre
    windowClass.hInstance = m_appInstance;              // Instance de l'application
    windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);   // Utilisation de l'icone par defaut
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);     // Curseur par defaut
    windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // Pas de fond specifique
    windowClass.lpszMenuName = 0;                       // Pas de menu associe
    windowClass.lpszClassName = L"DX12WindowClass";     // Nom de la classe de fenetre

    if (!RegisterClass(&windowClass)) // Enregistre la classe de fenetre avec RegisterClass
    {
        MessageBox(0, L"Erreur d'initialisation de la classe window.", 0, 0);
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions
    RECT R = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int width = R.right - R.left;
    int height = R.bottom - R.top;

    // Creation de la fenetre
    m_mainWindow = CreateWindow(L"DX12WindowClass", m_windowTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_appInstance, 0);
    
    if (!m_mainWindow)
    {
        MessageBox(0, L"CreateWindow Failed.", 0, 0);
        return false;
    }

    ShowWindow(m_mainWindow, SW_SHOW);
    UpdateWindow(m_mainWindow);

	return true;
}

bool WindowDX::Initialize()
{
    if (!InitWindow())
        return false;

    if (!InitDirect3D())
        return false;

    MessageBox(0, L"INIT REUSSI !", L"Init", MB_OK);
    return true;
}

int WindowDX::Run()
{
    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Otherwise, do animation/game stuff.
        Update();
        Draw();
    }

    return (int)msg.wParam;
}

void WindowDX::Update()
{
}

void WindowDX::Draw()
{
}

LRESULT WindowDX::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        // Gerer le redimensionnement de la fenetre
        break;
    case WM_KEYDOWN:
        // Gerer la pression de touche
        break;
        // Autres messages ici
    case WM_DESTROY:
        if (mD3DDevice) 
        {
            // Libere les ressources Direct3D, les objets COM
            mD3DDevice->Release();
        }
        PostQuitMessage(0); // Indique que l'application doit se terminer
        break;
    case WM_CLOSE:
        if (mD3DDevice)
        {
            // Libere les ressources Direct3D, les objets COM
            mD3DDevice->Release();
        }
        DestroyWindow(hwnd); // Ferme la fenetre
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam); // Traite les autres messages par defaut
    }

    return 0;
}

bool WindowDX::InitDirect3D()
{
    // Debug Log DirectX & Enable run-time memory check for debug build
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }
#endif

    // 1 Creation du factory DXGI
    ComPtr<IDXGIFactory4> dxgiFactory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))))
    {
        MessageBox(0, L"Erreur lors de la creation du DXGIFactory.", 0, 0);
        return false;
    }

    // 2 Creation du device DirectX12
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&mD3DDevice))))
    {
        MessageBox(0, L"Erreur lors de la creation du D3D12 Device.", 0, 0);
        return false;
    }

    // 3 Creation de la Command Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(mD3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue))))
    {
        MessageBox(0, L"Erreur lors de la creation de la Command Queue.", 0, 0);
        return false;
    }

    if (m_clientWidth <= 0 || m_clientHeight <= 0)
    {
        MessageBox(0, L"Les dimensions de la fenetre sont invalides.", 0, 0);
        return false;
    }

    // 4 Creation du Swap Chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_clientWidth;
    swapChainDesc.Height = m_clientHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = mFrameCount;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ComPtr<IDXGISwapChain1> swapChain;

    HRESULT commandResult = dxgiFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), m_mainWindow, &swapChainDesc, nullptr, nullptr, &swapChain);
    if (FAILED(commandResult))
    {
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"CreateSwapChainForHwnd failed with HRESULT 0x%08X.", commandResult);
        MessageBox(0, errorMsg, L"Swap Chain Error", MB_OK);
        return false;
    }
    swapChain.As(&mSwapChain); // convertion du swapchain1 au 3

    // 5 Creation du Descriptor Heap pour les Render Target Views (RTV)
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = mFrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(mD3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap))))
    {
        MessageBox(0, L"Erreur lors de la creation du RTV Heap.", 0, 0);
        return false;
    }

    // 6 Creation des Render Target Views pour chaque buffer du swap chain
    mRtvDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < mFrameCount; i++)
    {
        if (FAILED(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i]))))
        {
            MessageBox(0, L"Erreur lors de la recuperation du buffer du Swap Chain.", 0, 0);
            return false;
        }
        mD3DDevice->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, mRtvDescriptorSize);
    }

    // 7 Creation du CommandAllocator
    commandResult = mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
    if (FAILED(commandResult))
    {
        MessageBox(0, L"Erreur lors de la creation du Command Allocator.", 0, 0);
        return false;
    }

    // 8 Creation de la Command List
    commandResult = mD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
    if (FAILED(commandResult))
    {
        MessageBox(0, L"Erreur lors de la creation de la Command List.", 0, 0);
        return false;
    }
    mCommandList->Close(); // commandlist "en attente"

    // 9 Creation de fence
    commandResult = mD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
    if (FAILED(commandResult))
    {
        MessageBox(0, L"Erreur lors de la creation du Fence.", 0, 0);
        return false;
    }
    mFenceValue = 1;

    return true;
}
