#pragma once

#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <string>
#include "d3dx12.h"

using namespace Microsoft::WRL;

class TriangleRenderer
{
public:
    TriangleRenderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, IDXGISwapChain3* swapChain, ID3D12DescriptorHeap* rtvHeap);
    bool Initialize();
    void Update();
    void Render();

private:
    void CreatePipelineState();
    void CreateVertexBuffer();

    UINT m_RtvDescriptorSize;
    ComPtr<ID3D12Resource> m_RenderTargets[2];
    ComPtr<ID3D12Device> m_Device;
    ComPtr<ID3D12CommandQueue> m_CommandQueue;
    ComPtr<IDXGISwapChain3> m_SwapChain;
    ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
    ComPtr<ID3D12PipelineState> m_PipelineState;
    ComPtr<ID3D12RootSignature> m_RootSignature;
    ComPtr<ID3D12Resource> m_VertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

