
cbuffer ConstBuffer : register(b0)
{
    matrix viewProjMatrix;
};

struct VS_INPUT
{
    float3 posL : POSITION0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT; // 頂点カラー（R,G,B,A）
    float4 color : COLOR0; // ★ COLORセマンティクスで受け取る
    float2 texcoord : TEXCOORD0;
    
    // slot1: インスタンスバッファ
    float4 world0 : WORLD0;
    float4 world1 : WORLD1;
    float4 world2 : WORLD2;
    float4 world3 : WORLD3;

};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION; // 変換済み頂点座標
    float3 normal : NORMAL; 
    float4 color : COLOR; // 頂点カラー
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
   
    float4x4 worldMatrix = float4x4(vs_in.world0, vs_in.world1, vs_in.world2, vs_in.world3);
    
    float4 world = (mul(float4(vs_in.posL, 1), worldMatrix));
    vs_out.posH = mul(world, viewProjMatrix);

    vs_out.color = vs_in.color;
    vs_out.texcoord = vs_in.texcoord;
    vs_out.normal = vs_in.normal;
    return vs_out;
}