
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 PSMain(PS_INPUT input) : SV_Target // SV_Target est utilise pour le rendu moderne
{
    //return float4(0, 1, 0, 1); green par defaut
    return input.color; // Renvoie la couleur du pixel
}
