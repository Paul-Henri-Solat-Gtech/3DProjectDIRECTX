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
    windowClass.lpfnWndProc = MainWndProc;               // Fonction de callback pour gerer les messages
    windowClass.cbClsExtra = 0;                         // Pas d'espace supplementaire alloue pour cette classe
    windowClass.cbWndExtra = 0;                         // Pas d'espace supplementaire alloue pour chaque fenetre
    windowClass.hInstance = m_appInstance;                  // Instance de l'application
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

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int width = R.right - R.left;
    int height = R.bottom - R.top;

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

    //if (!InitDirect3D())
    //    return false;

    // Do the initial resize code.
    //OnResize();

    return true;
}

int WindowDX::Run()
{
    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Otherwise, do animation/game stuff.
        else
        {
            //mTimer.Tick();

            //if (!mAppPaused)
            //{
            //    CalculateFrameStats();
            //    Update(mTimer);
            //    Draw(mTimer);
            //}
            //else
            //{
            //    Sleep(100);
            //}
        }
    }

    return (int)msg.wParam;
}

LRESULT WindowDX::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        // Gerer le redimensionnement de la fenêtre
        break;
    case WM_KEYDOWN:
        // Gerer la pression de touche
        break;
        // Autres messages ici
    case WM_DESTROY:
        PostQuitMessage(0); // Indique que l'application doit se terminer
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam); // Traite les autres messages par defaut
    }

    return 0;
}