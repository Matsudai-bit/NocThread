
#include "SimpleShape2DInstansing.hlsli"
static const int vnum = 4;

static const float4 offset_array[vnum] =
{
    float4(-1.0f, 1.0f, 0.0f, 0.0f), //	左上
	float4(1.0f, 1.0f, 0.0f, 0.0f), //	右上
	float4(-1.0f, -1.0f, 0.0f, 0.0f), //	左下
	float4(1.0f, -1.0f, 0.0f, 0.0f), //	右下

};

[maxvertexcount(vnum)]
void main(
	point PS_INPUT input[1],
	inout TriangleStream<PS_INPUT> output
)
{

    for (int i = 0; i < vnum; i++)
    {
        PS_INPUT element;

        float4 res = offset_array[i];

        element.Pos = float4(0.0f, 0.0f, 0.0f, 1.0f) + offset_array[i];

        // angleを使用して回転させる
        element.Pos.x = offset_array[i].x * cos(angle.x) - offset_array[i].y * sin(angle.x);
        element.Pos.y = offset_array[i].x * sin(angle.x) + offset_array[i].y * cos(angle.x);
        element.Pos.z = 0.0f;

        element.Instance_Color = input[0].Instance_Color;
        element.Tex.x = offset_array[i].x + 1.0f;
        element.Tex.y = -offset_array[i].y + 1.0f;
        element.Tex /= 2.0f;
        
        element.ShapeID = input[0].ShapeID;
		
        output.Append(element);
    }
    output.RestartStrip();
}
