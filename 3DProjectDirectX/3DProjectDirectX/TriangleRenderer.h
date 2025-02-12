#pragma once

#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <string>
#include "d3dx12.h"
#include <DirectXMath.h>

using namespace Microsoft::WRL;

struct Vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT4 Color;
};

class TriangleRenderer
{
public:
    TriangleRenderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, ID3D12GraphicsCommandList* commandList, IDXGISwapChain3* swapChain, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize);
    bool Initialize();
    void Update();
    void Render();

    ID3D12PipelineState* GetPipelineState() const;
    ID3D12RootSignature* GetRootSignature() const;

private:
    void CreateVertexBuffer();
    void CreatePipelineState();

    UINT m_RtvDescriptorSize;
    ComPtr<ID3D12Resource> m_RenderTargets[2];
    ComPtr<ID3D12Device> m_Device;
    ComPtr<ID3D12CommandQueue> m_CommandQueue;
    ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    ComPtr<IDXGISwapChain3> m_SwapChain;
    ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
    ComPtr<ID3D12Resource> m_VertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

    ComPtr<ID3D12PipelineState> m_PipelineState;
    ComPtr<ID3D12RootSignature> m_RootSignature;
};

