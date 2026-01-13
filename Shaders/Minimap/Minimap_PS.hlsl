
Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);


cbuffer Parameters : register(b1)
{
    float4 transparentColor;
    float2 windowSize;

};




float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    
     // アスペクトの算出
    float2 aspect = float2(windowSize.x / windowSize.y, 1.0f);
    
    
    float4 col = Texture.Sample(TextureSampler, texCoord) * color;
    
    // transparentColorで指定した色を透明にする
    float isAlpha = step(0.01f, distance(transparentColor.rgb, col.rgb));
    
    col.a = lerp(0.3f, col.a, isAlpha);
        
    //float isAlpha = step(0.0f, distance(transparentColor, col));
    
    float2 scale = float2(1.35f, 1.35f);
    
    // 中心UVの算出（描画UV座標）
    float2 centerUV = float2(0.5f, 0.5f); // * aspect;
    
    // 距離の算出
    float2 dist = centerUV - texCoord * aspect ;
    
    // 拡大率を適用
    dist.x /= scale.x;
    dist.y /= scale.y;
    
    // 半径の算出
    float radius = length(dist);
    // 短くする(標準だとでかすぎる）
    radius = radius / 0.5f ;
        
    float alpha = lerp(1.0f, 0.0f, radius);
    
    // 円をくっきり指せる
    float flag = step(0.0f, alpha);
    
    float4 outputB = col * float4(1, 1, 1, flag);
  
    
    return outputB;
}
