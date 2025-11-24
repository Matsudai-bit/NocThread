#include "../SimpleShape2D.hlsli"


float4 main(PS_INPUT input) : SV_Target0
{
    // アスペクトの算出
    float2 aspect = float2(windowSize.x / windowSize.y, 1.0f);
    
    // 中心UVの算出（描画UV座標）
    float2 centerUV = (input.Position2D.xy / windowSize); // * aspect;
    
    float2 size = float2(0.5f, 0.5f) * scale * aspect;
    
    float isInsideX = step(centerUV.x - size.x, input.Tex.x) - step(centerUV.x + size.x, input.Tex.x);
    float isInsideY = step(centerUV.y - size.y, input.Tex.y) - step(centerUV.y + size.y, input.Tex.y);
    
    
    // 5. 最終判定: X軸もY軸も範囲内であれば描画フラグを1にする
    float isInsideQuad = isInsideX * isInsideY; // 論理積 (AND)

    // 6. 出力
    // isInsideQuad が 1 のときのみ、元の色 (Color) を出力
    float4 output = input.Color * float4(1.0f, 1.0f, 1.0f, isInsideQuad);

    return output;
}
