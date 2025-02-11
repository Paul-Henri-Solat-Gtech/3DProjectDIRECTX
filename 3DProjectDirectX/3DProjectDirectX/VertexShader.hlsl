//cbuffer Transform : register(b0) // Buffer de constantes contenant la matrice de transformation
//{
//    matrix WorldMatrix;
//};

struct VS_INPUT
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION; // SV_POSITION est utilisé pour les shaders modernes
    float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = float4(input.pos, 1.0f);
    // output.pos = mul(float4(input.pos, 1.0f), WorldMatrix); // Appliquer la transformation
    output.color = input.color;
    return output;
}
