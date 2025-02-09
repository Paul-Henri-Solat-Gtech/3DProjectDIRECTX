#include <iostream>
#include <DirectXColors.h>
#include "WindowDX.h"
#include "TriangleRenderer.h"

class InitDirect3DApp : public WindowDX
{
public:
    InitDirect3DApp(HINSTANCE hInstance);
    bool Initialize();
    void Update() override;
    void Draw() override;

private:
    //std::unique_ptr<TriangleRenderer> m_TriangleRenderer;

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

}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) : WindowDX(hInstance)
{
}

bool InitDirect3DApp::Initialize()
{
    if (!WindowDX::Initialize())
        return false;

    //m_TriangleRenderer = std::make_unique<TriangleRenderer>(mD3DDevice.Get(), mCommandQueue.Get(), mSwapChain.Get(), mRtvHeap.Get());
    //if (!m_TriangleRenderer->Initialize())
    //    return false;

    return true;
}
void InitDirect3DApp::Update()
{
    // Update logic for the triangle
    //m_TriangleRenderer->Update();
}

void InitDirect3DApp::Draw()
{
    // Reinitialise le command allocator et la command list
    HRESULT hr = mCommandAllocator->Reset();
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur lors du Reset du Command Allocator.", 0, 0);
        return;
    }
    hr = mCommandList->Reset(mCommandAllocator.Get(), nullptr);
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur lors du Reset de la Command List.", 0, 0);
        return;
    }

    // Render the triangle
    //m_TriangleRenderer->Render();
    
    // Changer la couleur
    FLOAT clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // Bleu (RGBA)

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    // Prendre le handle de la vue du tampon de rendu
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRtvDescriptorSize);

    // Commencer la commande de dessin
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(),D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &barrier);

    // Effacer le tampon de rendu avec la couleur de fond
    mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // Fermer la Command List
    mCommandList->Close();

    // Soumettre la commande
    ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // --- Signal et synchronisation via le fence
    hr = mCommandQueue->Signal(mFence.Get(), mFenceValue);
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur lors du Signal du Fence.", 0, 0);
        return;
    }

    // Attendre que le GPU atteigne le fence
    if (mFence->GetCompletedValue() < mFenceValue)
    {
        HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        hr = mFence->SetEventOnCompletion(mFenceValue, eventHandle);
        if (FAILED(hr))
        {
            MessageBox(0, L"Erreur lors du SetEventOnCompletion du Fence.", 0, 0);
            return;
        }
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
    mFenceValue++;

    // Presenter le SwapChain (1 pour V-Sync)
    mSwapChain->Present(1, 0);
}
