

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;

};


float4 main(PS_INPUT input) : SV_Target0
{
    return float4(1, 1, 1, 1);
}