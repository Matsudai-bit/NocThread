#include "ConcentrationLines.hlsli"

float2 GradientNoiseDir(float2 p)
{
    p = p % 289;
    float x = (34 * p.x + 1) * p.x % 289 + p.y;
    x = (34 * x + 1) * x % 289;
    x = frac(x / 41) * 2 - 1;
    return normalize(float2(x - floor(x + 0.5), abs(x) - 0.5));
}

float GradientNoise(float2 p)
{
    float2 ip = floor(p);
    float2 fp = frac(p);
    float d00 = dot(GradientNoiseDir(ip), fp);
    float d01 = dot(GradientNoiseDir(ip + float2(0, 1)), fp - float2(0, 1));
    float d10 = dot(GradientNoiseDir(ip + float2(1, 0)), fp - float2(1, 0));
    float d11 = dot(GradientNoiseDir(ip + float2(1, 1)), fp - float2(1, 1));
    fp = fp * fp * fp * (fp * (fp * 6 - 15) + 10);
    return lerp(lerp(d00, d01, fp.y), lerp(d10, d11, fp.y), fp.x);
}

float GradientNoiseScale(float2 UV, float Scale)
{
    return GradientNoise(UV * Scale) + 0.5;
}

float2 ToPolar(float2 xy)
{
    float r = length(xy);
    float rad = atan2(xy.x, xy.y);
    return float2(r, rad);
}



float4 main(PS_INPUT input) : SV_TARGET
{
    //return float4(1.0f, 0.f, 0.f, 1.f);
    float2 uv = ToPolar(input.Tex - uvOffset);
    float length = uv.r * lineRate;
    float rad = (uv.g / (3.1415926535) + 1.0f) / 2.0f;

    float noise = GradientNoiseScale(
		float2(rad, rad) + float2(time * speed, 0.0f), noiseScale
	);

    float y = 1.0f - length + noise;
    float region = smoothstep(lineRegion, 1.0f, y);

    float3 color = float3(1.f, 1.f, 1.f);
    color = lerp(lineColor, color, region.r);
    return float4(color,  1.f - region.r);
    
}