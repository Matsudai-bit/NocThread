
cbuffer ConstBuffer : register(b0)
{
    float2 angle;
    float2 scale;
    float2 windowSize;
    
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

struct GS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Position2D : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;

};

struct PS_INPUT
{
    float4 Pos : SV_POSITION ;
    float3 Position2D : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;

};