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

    // 三角形の頂点座標の算出　時計回りになるように配置 
    float2 triangleVertex[3] =
    {
        scale * float2(0.f, -0.5f) + windowCenterUV,
        scale * float2(0.5f, 0.5f) + windowCenterUV,
        scale * float2(-0.5f, 0.5f) + windowCenterUV,
    };
    // 各辺のベクトル算出
    float2 edge[3] =
    {
        triangleVertex[1] - triangleVertex[0],
        triangleVertex[2] - triangleVertex[1],
        triangleVertex[0] - triangleVertex[2]
    };

    float isOutside = 0.0f;
    // 内外判定
    [unroll] // ループを物理的に展開して分岐を消去するようコンパイラに指示
    for (int i = 0; i < 3; i++)
    {
        // 辺ベクトルと頂点からのベクトルの外積を算出
        float2 toPoint = input.Tex - triangleVertex[i];
        float cro = cross2d(edge[i], toPoint);
        
        // 負の場合は外側
         isOutside += step(0.0f, -cro) ;
    }

    return input.Color * float4(1.0f, 1.0f, 1.0f, 1.0f - isOutside);

  
}
