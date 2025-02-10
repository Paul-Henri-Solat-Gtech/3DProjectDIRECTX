#include "TriangleRenderer.h"
#include <d3dcompiler.h>

TriangleRenderer::TriangleRenderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, IDXGISwapChain3* swapChain, ID3D12DescriptorHeap* rtvHeap)
    : m_Device(device), m_CommandQueue(commandQueue), m_SwapChain(swapChain), m_RtvHeap(rtvHeap)
{
}

bool TriangleRenderer::Initialize()
{
    CreatePipelineState();
    CreateVertexBuffer();
    return true;
}

void TriangleRenderer::Update()
{
    // Update logic (if needed)
}

void TriangleRenderer::Render()
{
    // Get the back buffer index
    UINT backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

    // Set up the render target
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_CommandList->ResourceBarrier(1, &barrier);

    // Get the render target view handle
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, m_RtvDescriptorSize);

    // Clear the render target
    FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // Set the pipeline state and root signature
    m_CommandList->SetPipelineState(m_PipelineState.Get());
    m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

    // Set primitive topology
    m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

    // Draw the triangle
    m_CommandList->DrawInstanced(3, 1, 0, 0);

    // Transition back to present state
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_CommandList->ResourceBarrier(1, &barrier);
}

void TriangleRenderer::CreatePipelineState()
{
    // Create root signature
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));

    // Load and compile shaders
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    D3DCompileFromFile(L"DefaultShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertexShader, nullptr);
    D3DCompileFromFile(L"DefaultShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixelShader, nullptr);

    // Define the input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Create the pipeline state object
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = m_RootSignature.Get();
    psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
    psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
}

void TriangleRenderer::CreateVertexBuffer()
{
    // Define vertices of a triangle
    struct Vertex {
        float position[3];
    };
    Vertex triangleVertices[] = {
        { { 0.0f,  0.5f, 0.0f } },
        { { 0.5f, -0.5f, 0.0f } },
        { { -0.5f, -0.5f, 0.0f } }
    };

    const UINT vertexBufferSize = sizeof(triangleVertices);

    // Create vertex buffer
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    m_Device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertexBuffer));

    // Copy data to vertex buffer
    void* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    m_VertexBuffer->Map(0, &readRange, &pVertexDataBegin);
    memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
    m_VertexBuffer->Unmap(0, nullptr);

    // Initialize vertex buffer view
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
    m_VertexBufferView.StrideInBytes = sizeof(Vertex);
    m_VertexBufferView.SizeInBytes = vertexBufferSize;
}


