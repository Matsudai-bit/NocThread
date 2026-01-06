
Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

Texture2D<float4> MaskTexture : register(t1);

cbuffer Parameters : register(b1)
{
    float Rate;
};

float4 getFirstColor(float4 color , float2 texCoord)
{
    float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Texture.Sample(TextureSampler, texCoord) * color;

    float2 uv = texCoord;
    
    float rate = Rate;
 
    uv.x *= 16.0f;
    uv.y *= 9.0f;
    
    int x = trunc(uv.x) % 2;
    int y = trunc(uv.y) % 2;
    
    int r = x ^ y; // 排他的論理和
    
    //float alpha = lerp(Rate * 4 - frac(uv.x), Rate * 4 - frac(uv.x) - 2.0f, r);
    
    float alpha = rate * 4 - frac(uv.x) - r * 2.0f;
    
    return float4(col.rgb, alpha);
}

float4 getSecondColor(float4 baseColor, float4 maskColor, float2 texCoord)
{
    float2 uv = texCoord;
    float4 color2 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float2 aspectValue = float2(1280.0f / 720.0f, 1.0f);
    
   //uv *= aspectValue;
    
    float splits = 8.0f;
    
    float nX = floor(uv.x * splits);
    float nY = floor(uv.y * splits);
    
    //float lengthd = distance(float3(nX, nY),  float3(3.0f, 3.0f));
    
    float halfV = (splits - 1.0f) * 0.5f;
    nX = floor(abs(nX - halfV)) + 1.0f;
    nY = floor(abs(nY - halfV)) + 1.0f;
    
    
    float alpha = (nX + nY) - (2.0f + splits) * Rate;
    
    alpha = max(0.0f, alpha);

        
    return lerp(baseColor, maskColor, step(alpha, 0));
}

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 col = Texture.Sample(TextureSampler, texCoord) * color;
    float4 mask = MaskTexture.Sample(TextureSampler, texCoord);

    // 横方向にワイプ
//    float rate = saturate((texCoord.x - 1.0f) + Rate * 2.0f);
//    float rate = saturate(-texCoord.x + Rate * 2.0f);

    // マスク用テクスチャを使用
    //float rate = saturate((mask.x - 1.0f) + Rate * 2.0f);
    float rate = saturate(-mask.x + Rate * 2.0f);
    
    //ここから鬼滅の刃 == == == == == == == == == == == == =

    
    return lerp(getFirstColor(col, texCoord), getSecondColor(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f), texCoord), 1.0f);
    // ここまで =====================================
    
    

    //float alpha = 1.0f;
    //float lengthValue = distance(float2(0.5f, 0.5f), texCoord);
    
    ////alpha = lerp(1.0f, 0.5f, step(0.2f, Rate - lengthValue));
    
    //if ( Rate  < lengthValue)
    //{
    //    alpha = 0.0f;
    //}
    
    //   return float4(col.rgb, alpha);
}
