

struct PS_INPUT
{
    float4 pos : SV_POSITION; // 変換済み頂点座標
    float3 normal : NORMAL;
    float4 color : COLOR; // 頂点カラー
    float2 texcoord : TEXCOORD0;

};


float4 main(PS_INPUT input) : SV_Target0
{
    return float4(input.color);
}