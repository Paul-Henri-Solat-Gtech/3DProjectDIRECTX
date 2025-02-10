// DefaultShader.hlsl

// Structure d'entrée pour le vertex shader
struct VS_Input
{
    float3 position : POSITION;
};

// Structure de sortie pour le pixel shader
struct PS_Input
{
    float4 position : SV_POSITION;
};

// Vertex Shader
PS_Input VSMain(VS_Input input)
{
    PS_Input output;
    output.position = float4(input.position, 1.0f); // Transformation de la position du vertex
    return output;
}

// Pixel Shader
float4 PSMain() : SV_TARGET
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f); // Rose (R: 1.0, G: 0.0, B: 1.0, A: 1.0)
}