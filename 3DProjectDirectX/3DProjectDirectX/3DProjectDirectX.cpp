#include <iostream>
#include <DirectXColors.h>
#include "WindowDX.h"

class InitDirect3DApp : public WindowDX
{
public:
    InitDirect3DApp(HINSTANCE hInstance);
    bool Initialize();

private:

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    InitDirect3DApp theApp(hInstance);
    if (!theApp.Initialize())
        return 0;
    return theApp.Run();

    //try
    //{
    //    InitDirect3DApp theApp(hInstance);
    //    if (!theApp.Initialize())
    //        return 0;

    //    return theApp.Run();
    //}
    //catch (int error)
    //{
    //    //MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    //    return 0;
    //}
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) : WindowDX(hInstance)
{

}
bool InitDirect3DApp::Initialize()
{
    if (!WindowDX::Initialize())
        return false;

    return true;
}