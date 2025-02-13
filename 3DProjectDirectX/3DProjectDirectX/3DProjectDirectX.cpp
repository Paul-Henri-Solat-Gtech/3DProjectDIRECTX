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
    TriangleRenderer* m_TriangleRenderer;
    ComPtr<ID3D12PipelineState> mPSO;
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

 /*   m_TriangleRenderer = std::make_unique<TriangleRenderer>(mD3DDevice.Get(), mCommandQueue.Get(),mCommandList.Get(), mSwapChain.Get(), mRtvHeap.Get(),mRtvDescriptorSize);
    if (!m_TriangleRenderer->Initialize())
        return false;*/

    m_TriangleRenderer = new TriangleRenderer(mD3DDevice.Get(), mCommandQueue.Get(), mCommandList.Get(), mSwapChain.Get(), mRtvHeap.Get(), mRtvDescriptorSize);
    if (!m_TriangleRenderer->Initialize())
    {
        delete m_TriangleRenderer;  // Liberation si l'initialisation echoue
        m_TriangleRenderer = nullptr;
        return false;
    }
    mPSO = m_TriangleRenderer->GetPipelineState();


    return true;
}
void InitDirect3DApp::Update()
{
    // Update logic for the triangle
    m_TriangleRenderer->Update();
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
    hr = mCommandList->Reset(mCommandAllocator.Get(), nullptr); // rajouter le pipeline state
    if (FAILED(hr))
    {
        MessageBox(0, L"Erreur lors du Reset de la Command List.", 0, 0);
        return;
    }

    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Prendre le handle de la vue du tampon de rendu
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);

    // Commencer la commande de dessin BARRIER START
    CD3DX12_RESOURCE_BARRIER barrierStart = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &barrierStart);

    FLOAT clearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    mCommandList->OMSetRenderTargets(1, &rtvHandle, true, nullptr);


    // Appel pour dessiner le triangle
    //mCommandList->SetGraphicsRootSignature(m_TriangleRenderer->GetRootSignature());
    //mCommandList->SetPipelineState(m_TriangleRenderer->GetPipelineState());

    m_TriangleRenderer->Render(); // Rendu du triangle ici

    // Transition du back buffer de RENDER_TARGET PRESENT avant de le presenter BARRIER STOP
    CD3DX12_RESOURCE_BARRIER barrierStop = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &barrierStop);

    // Fermer la Command List
    mCommandList->Close();

    // Soumettre la commande
    ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    FlushCommandQueue();

    // Presenter le SwapChain (1 pour V-Sync) swap the back & front buffer
    mSwapChain->Present(0, 0);
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;


}