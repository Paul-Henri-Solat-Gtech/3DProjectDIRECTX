#include "TriangleRenderer.h"
#include <d3dcompiler.h>

TriangleRenderer::TriangleRenderer(ID3D12Device* device, ID3D12CommandQueue* commandQueue, ID3D12GraphicsCommandList* commandList, IDXGISwapChain3* swapChain, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize)
    : m_Device(device), m_CommandQueue(commandQueue),m_CommandList(commandList), m_SwapChain(swapChain), m_RtvHeap(rtvHeap),m_RtvDescriptorSize(rtvDescriptorSize)
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

    // Get the render target view handle
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, m_RtvDescriptorSize);

    //// Clear the render target
 

    // Set the pipeline state and root signature
    m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
    m_CommandList->SetPipelineState(m_PipelineState.Get());
 

    // Set primitive topology

    m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // Draw the triangle
    m_CommandList->DrawInstanced(3, 1, 0, 0);
    //MessageBox(0, L"Triangle is here !", L"Draw", MB_OK);
}

void TriangleRenderer::CreateVertexBuffer()
{
    Vertex m_triangleVertices[] =
    {
        { DirectX::XMFLOAT3(0.0f, 0.5f, 5.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // Haut (Rouge)
        { DirectX::XMFLOAT3(0.5f, -0.5f, 5.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // Bas droit (Vert)
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }  // Bas gauche (Bleu)
    };

    const UINT vertexBufferSize = sizeof(m_triangleVertices);

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
    m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, m_triangleVertices, vertexBufferSize);
    m_VertexBuffer->Unmap(0, nullptr);

    // Initialize vertex buffer view
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
    m_VertexBufferView.StrideInBytes = sizeof(Vertex);
    m_VertexBufferView.SizeInBytes = vertexBufferSize;
}

void TriangleRenderer::CreatePipelineState()
{
    // Create root signature
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature = nullptr;
    ComPtr<ID3DBlob> error = nullptr;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    HRESULT h = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));

    // Load and compile shaders
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error);
    if (FAILED(hr))
    {
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"D3DCompileFromFileVertex failed with HRESULT 0x%08X.", hr);
        MessageBox(0, errorMsg, L"D3DCompileFromFile Error", MB_OK);
    }

    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error);
    if (FAILED(hr))
    {
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"D3DCompileFromFilePixel failed with HRESULT 0x%08X.", hr);
        MessageBox(0, errorMsg, L"D3DCompileFromFile Error", MB_OK);
    }

    // Define the input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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

    hr = m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
    if (FAILED(hr))
    {
        MessageBox(0, L"CreateGraphicsPipelineState failed.", L"Error", MB_OK);
        return;
    }
}

ID3D12PipelineState* TriangleRenderer::GetPipelineState() const
{
    return m_PipelineState.Get();
}

ID3D12RootSignature* TriangleRenderer::GetRootSignature() const
{
    return m_RootSignature.Get();
}