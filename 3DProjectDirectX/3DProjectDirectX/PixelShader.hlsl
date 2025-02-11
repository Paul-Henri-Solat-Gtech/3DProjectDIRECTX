struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 PSMain(PS_INPUT input) : SV_Target // SV_Target est utilis� pour le rendu moderne
{
    return input.color; // Renvoie la couleur du pixel
}
