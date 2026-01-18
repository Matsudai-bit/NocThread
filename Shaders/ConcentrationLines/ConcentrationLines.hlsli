cbuffer ConstBuffer	: register(b0)
{
    float3  lineColor   : packoffset(c0);
    float   time        : packoffset(c0.w);
    float   speed       : packoffset(c1.x);
    float   noiseScale  : packoffset(c1.y);
    float2  uvOffset    : packoffset(c1.z);
    float   lineRegion  : packoffset(c2);
};

struct VS_INPUT
{
	float3 Pos : POSITION;
	float4 Color: COLOR;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color: COLOR;
	float2 Tex : TEXCOORD;
};