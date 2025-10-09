#include "CircleShadow.hlsli"


float4 main(PS_INPUT input) : SV_Target0
{
    
    float2 centerPosition = float2(0.5f, 0.5f);
    
    float2 dist = centerPosition - input.Tex;
    
    float lengthValue = length(dist); //dist.x * dist.x + dist.y * dist.y;
    
    
    float lerpValue = lengthValue / 0.25f;
    
    float alpha = lerp(1.0f * alphaValue.x, 0.0f, lerpValue);
    
    float4 outputB = float4(0, 0, 0, alpha );
    

    
    return outputB;

}
