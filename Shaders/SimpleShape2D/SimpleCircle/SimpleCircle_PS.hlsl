#include "../SimpleShape2D.hlsli"

float cross2d(float2 a, float2 b)
{
    return a.x * b.y - a.y * b.x;
}


float4 main(PS_INPUT input) : SV_Target0
{
    
    // アスペクトの算出
    float2 aspect = float2(windowSize.x / windowSize.y, 1.0f);
    
    // 配置中心UVの算出（描画UV座標）
    float2 windowCenterUV = (input.Position2D.xy / windowSize);


    // 距離の算出
    float2 dist = windowCenterUV - input.Tex * aspect;
    
    // 拡大率を適用
    dist.x /= scale.x;
    dist.y /= scale.y;
    
    // 半径の算出
    float radius = length(dist);
    // 短くする(標準だとでかすぎる）
    radius = radius / 0.5f;
        
    float alpha = lerp(1.0f, 0.0f, radius);
    
    // 円をくっきり指せる
    float flag = step(0.0f, alpha);
    
    float4 outputB = input.Color * float4(1, 1, 1, flag);
    

    
    return outputB;

}
