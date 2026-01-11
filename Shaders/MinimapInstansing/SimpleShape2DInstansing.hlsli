
cbuffer ConstBuffer : register(b0)
{
    float2 angle;
    float2 scale;
    float2 windowSize;
    
};

struct VS_INPUT
{
   // slot 0 : 頂点座標
    float3 Pos : POSITION;
    float4 Color : COLOR0;
    float2 Tex : TEXCOORD;
    
    // slot 1 : インスタンス行列の1列目
    float4 World0 : WORLD0;
    float4 World1 : WORLD1;
    float4 World2 : WORLD2;
    float4 World3:  WORLD3;
    float4 Instance_Color : COLOR1;
    
    float ShapeID : NORMAL;
};

struct GS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;

};

struct PS_INPUT
{
    float4 Pos : SV_POSITION ;
    float4 Instance_Color : COLOR;
    float2 Tex : TEXCOORD;
    
    float ShapeID : NORMAL;


};