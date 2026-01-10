#include "../SimpleShape2DInstansing.hlsli"


float4 main(PS_INPUT input) : SV_Target0
{
    // 1. アスペクト比 (横 / 縦)
    float aspect = windowSize.x / windowSize.y;

    // 2. 基準サイズ (例: 縦を基準に 0.5 の大きさ)
    float2 baseSize = float2(0.5f, 0.5f);

    // 3. 描画サイズ（横方向をアスペクト比で割ることで、見た目上の正方形を作る）
    // 横に広い画面ほど、UV空間での x の範囲を小さくする必要がある
    float2 size = float2(baseSize.x / aspect, baseSize.y);

    float2 centerUV = float2(0.5f, 0.5f);

    // 4. 範囲判定 (step関数)
    float2 lowBound = centerUV - size * 0.5f; // sizeを「幅」とするなら0.5を掛ける
    float2 highBound = centerUV + size * 0.5f;

    float2 isInside = step(lowBound, input.Tex) - step(highBound, input.Tex);
    float isInsideQuad = isInside.x * isInside.y;

    // 5. 出力
    return input.Instance_Color * float4(1.0f, 1.0f, 1.0f, isInsideQuad);
}
