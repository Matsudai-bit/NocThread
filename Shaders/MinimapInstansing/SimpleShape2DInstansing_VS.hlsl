#include "SimpleShape2DInstansing.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    // 1. インスタンス行列の再構成 (4x4行列にする)
    // C++側で Row-Major (行優先) で送っている場合、そのまま float4x4 に代入します
    float4x4 worldMatrix = float4x4(
        input.World0,
        input.World1,
        input.World2,
        input.World3
    );

    // 2. ローカル座標をワールド座標へ変換
    float4 pos = mul(float4(input.Pos, 1.0f), worldMatrix);
    // 3. 座標系の変換 (重要！)
    // ミニマップ用座標(ピクセルなど)から正規化デバイス座標(NDC: -1.0 ～ 1.0)へ変換
    // windowSize がピクセル単位(例: 1280, 720)の場合
    
    // アスペクト比
    float2 aspect = float2(windowSize.x / windowSize.y, 1.0f);
    
  
    output.Pos = pos;
    
    
    //output.Pos = float4(input.Pos.xy, 0.0f, 1.0f); // 4. その他のデータを渡す
    //output.Pos = pos;
    
    output.Instance_Color= input.Instance_Color;
    
    // UV座標を回転させる
    output.Tex = input.Tex;
    
    
    
    // ピクセルシェーダでの判定用に、ワールド空間（または正規化空間）の座標を渡す
    output.ShapeID = input.ShapeID;
    return output;
}

