
#include "SimpleParticle.hlsli"

static const int vnum = 4;

static const float4 offset_array[vnum] =
{
    float4(-0.5f,-0.5f, 0.0f, 0.0f), // 左上
	float4(0.5f ,-0.5f , 0.0f, 0.0f), // 右上
	float4(-0.5f, 0.5f , 0.0f, 0.0f), // 左下
	float4(0.5f , 0.5f  , 0.0f, 0.0f), // 右下

};

[maxvertexcount(vnum)]
void main(
	point GS_INPUT input[1],
	inout TriangleStream<PS_INPUT> output
)
{
    // ターゲット方向を計算
    float3 forward = normalize(objectTarget.xyz - input[0].Pos.xyz); // Z方向に向けたい方向

	// 適当なupベクトル（Y軸）との外積で右方向を求める
    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, forward));
    float3 newUp = normalize(cross(forward, right));

    
	// ローカル空間→ワールド空間への回転行列を構築
    float3x3 rotation = float3x3(right, newUp, float3(0.0f, 0.0f, 1.0f));
    
  
    //	入力として渡された頂点1つに対して、4つに増やす作業を行う
    for (int i = 0; i < vnum; i++)
    {
        PS_INPUT element;

	    //	頂点シェーダが出力した座標を変数に確保
	    //	※実際には使っていない
        float4 pos = input[0].Pos;
        
        // オフセットに回転を加える
        float3 worldOffset = mul(offset_array[i].xyz * scale, rotation);

	    //	頂点シェーダが出力した座標に、増殖させるポリゴンのi番目の座標を計算する
        element.Pos = float4(input[0].Pos.xyz + worldOffset, 1.0f);
        //element.Pos = float4(input[0].Pos + offset_array[i]);
		
		element.Pos = mul(element.Pos, matWorld);
        

	    //	上記の結果に、ビュー行列を掛け算する
        element.Pos = mul(element.Pos, matView);
	    //	上記の結果に、プロジェクション行列を掛け算する
      //  element.Pos = mul(element.Pos, matProj);

        element.Color = input[0].Color;
        
        element.Tex.x = offset_array[i].x + 1.0f;
        element.Tex.y = offset_array[i].y + 1.0f;
        element.Tex /= 2.0f;
	    //	ストリームへ頂点情報を追加する		
        output.Append(element);
    }
    

    output.RestartStrip();

}