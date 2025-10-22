
cbuffer ConstBuffer : register(b0)
{
    matrix viewProjMatrix;
};

struct VS_INPUT
{
    // slot0: 頂点バッファ
    float3 posL :       POSITION0; // 頂点座標
    float4 color :      COLOR0; // 頂点カラー（R,G,B,A）
    float2 texcoord : TEXCOORD0; // テクスチャ座標
    
    // slot1: インスタンスバッファ
    float4 world0 : WORLD0;
    float4 world1 : WORLD1;
    float4 world2 : WORLD2;
    float4 world3 : WORLD3;

};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION; // 変換済み頂点座標
    float4 color : COLOR0; // 頂点カラー
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;

    float4x4 worldMatrix = float4x4(vs_in.world0, vs_in.world1, vs_in.world2, vs_in.world3);
    
    // ★★★ 外部からビュー・プロジェクション行列を取得するための定数バッファが必要 ★★★
    // ここでは仮に 'viewProjMatrix' というグローバル変数があるとする
    // WVP行列
   // float4x4 wvpMatrix = mul(worldMatrix, viewProjMatrix); // ★★★ 仮の行列名 'viewProjMatrix' を使用 ★★★
    
    //// 頂点変換
    // 頂点座標に行列を適用し、クリップ空間座標を posH に格納
    //vs_out.posH = mul(float4(vs_in.posL, 1), worldMatrix);
    //vs_out.posH = mul(vs_out.posH, viewProjMatrix);
    
    // vs_out.posH = mul(vs_in.posL, worldMatrix); // ★★★ 以前の誤った行 ★★★
    // vs_out.posH = worldPos;                     // ★★★ 以前の誤った行 ★★★
    vs_out.posH = mul(mul(float4(vs_in.posL, 1),worldMatrix), viewProjMatrix);

    vs_out.color = vs_in.color;
    vs_out.texcoord = vs_in.texcoord;
    return vs_out;
}