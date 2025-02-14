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

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj;
};

class TriangleRenderer
{
public:
    TriangleRenderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, ID3D12GraphicsCommandList* commandList, IDXGISwapChain3* swapChain, ID3D12DescriptorHeap* rtvHeap, ID3D12DescriptorHeap* dsvHeap, UINT rtvDescriptorSize, float size, DirectX::XMFLOAT4 color, D3D12_DEPTH_STENCIL_DESC depthStencilDesc);
    bool Initialize();
    void Update();
    void UpdateTransform();
    void Render();

    ID3D12PipelineState* GetPipelineState() const;
    ID3D12RootSignature* GetRootSignature() const;

private:
    void CreateVertexBuffer(float size, DirectX::XMFLOAT4 color);
    void CreatePipelineState();
    void CreateConstantBuffer();

    float m_Size;
    DirectX::XMFLOAT4 m_Color;

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
    ComPtr<ID3D12Resource> m_IndexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

    ComPtr<ID3D12PipelineState> m_PipelineState;
    ComPtr<ID3D12RootSignature> m_RootSignature;

    ComPtr<ID3D12Resource> m_ConstantBuffer;
    ObjectConstants m_ObjectConstants;
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_CbvDesc;

    ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
    D3D12_DEPTH_STENCIL_DESC m_DepthStencilDesc;


};

