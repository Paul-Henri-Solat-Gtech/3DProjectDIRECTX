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
    void FlushCommandQueue();

private:
    std::unique_ptr<TriangleRenderer> m_TriangleRenderer;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
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

    FlushCommandQueue();

    m_TriangleRenderer = std::make_unique<TriangleRenderer>(mD3DDevice.Get(), mCommandQueue.Get(),mCommandList.Get(), mSwapChain.Get(), mRtvHeap.Get());
    if (!m_TriangleRenderer->Initialize())
        return false;

    return true;
}
void InitDirect3DApp::Update()
{
    // Update logic for the triangle
    m_TriangleRenderer->Update();
}

void InitDirect3DApp::Draw()
{
    assert(mD3DDevice);
    assert(mSwapChain);
    assert(mCommandAllocator);

    FlushCommandQueue();

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

    //mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    // Prendre le handle de la vue du tampon de rendu
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRtvDescriptorSize);

    // Commencer la commande de dessin
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(),D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &barrier);

    // Appel pour dessiner le triangle
    m_TriangleRenderer->Render(); // Rendu du triangle ici

    // Transition du back buffer de RENDER_TARGET à PRESENT avant de le présenter
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &barrier);

    // Fermer la Command List
    mCommandList->Close();

    // Soumettre la commande
    ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    FlushCommandQueue();

    // Presenter le SwapChain (1 pour V-Sync)
    mSwapChain->Present(1, 0);

    //mFrameIndex = 1 - mFrameIndex;
    mFrameIndex += 1;

    FlushCommandQueue();
}

void InitDirect3DApp::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    mFenceValue++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    HRESULT hr = mCommandQueue->Signal(mFence.Get(), mFenceValue);
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur lors du Signal du Fence.", 0, 0);
        return;
    }

    // Wait until the GPU has completed commands up to this fence point.
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
}