#include "SimpleShape2DInstansing.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

// 1. 行列の再構成
    float4x4 worldMatrix = float4x4(input.World0, input.World1, input.World2, input.World3);

    // 2. 回転・移動（ピクセル単位のワールド空間）
    float4 worldPos = mul(float4(input.Pos, 1.0f), worldMatrix);


   output.Pos = worldPos;
    

    
    // 4. 付随データの受け渡し
    output.Instance_Color = input.Instance_Color;
    output.Tex = input.Tex;
    output.Aspect = windowSize.x / windowSize.y; // 横/縦 の比率
    output.ShapeID = input.ShapeID; // C++側で定義している場合
    return output;
}

