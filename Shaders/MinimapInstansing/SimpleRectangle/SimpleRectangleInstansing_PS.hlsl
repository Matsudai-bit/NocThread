#include "../SimpleShape2DInstansing.hlsli"

float4 getRectangle(float2 uv, float aspect, float4 color)
{
      // 2. 基準サイズ (例: 縦を基準に 0.5 の大きさ)
    float2 baseSize = float2(0.5f, 0.5f);

    // 3. 描画サイズ（横方向をアスペクト比で割ることで、見た目上の正方形を作る）
    // 横に広い画面ほど、UV空間での x の範囲を小さくする必要がある
    float2 size = float2(baseSize.x / aspect, baseSize.y);

    float2 centerUV = float2(0.5f, 0.5f);

    // 4. 範囲判定 (step関数)
    float2 lowBound = centerUV - size * 0.5f; // sizeを「幅」とするなら0.5を掛ける
    float2 highBound = centerUV + size * 0.5f;

    float2 isInside = step(lowBound, uv) - step(highBound, uv);
    float isInsideQuad = isInside.x * isInside.y;

    // 5. 出力
    return color * float4(1.0f, 1.0f, 1.0f, isInsideQuad);
}

float4 getCircle(float2 uv, float aspect, float4 color)
{
    // 1. UVを -1.0 ～ 1.0 の範囲に変換（中心を 0,0 にする）
    float2 p = uv * 2.0f - 1.0f;

    // 2. 横方向にアスペクト比を掛けて空間を補正
    // これにより、横長の画面でも 1.0 の距離が等しくなる
    p.x *= aspect;

    // 3. 中心（0,0）からの距離を算出
    float dist = length(p);

    // 4. 半径の定義（0.0 ～ 1.0 の範囲で指定。0.5なら四角形の半分）
    float radius = 0.5f;

    // 5. 判定: 距離が半径より小さければ 1, 大きければ 0
    // step(A, B) は A <= B なら 1 を返す
    float flag = step(dist, radius);

    // 6. 出力
    return color * float4(1, 1, 1, flag);
}
float cross2d(float2 a, float2 b)
{
    return a.x * b.y - a.y * b.x;
}

float4 getTriangle(float2 uv, float aspect, float4 color)
{
    // 1. UVを中心基準 (-1.0 ~ 1.0) に変換
    float2 p = uv * 2.0f - 1.0f;
    
    // 2. 横方向にアスペクト比を掛けて空間を補正
    p.x *= aspect;

    // 3. 補正された空間上での三角形の頂点 (中心 0,0 基準)
    // 時計回りに配置
    float2 v[3] =
    {
        float2(0.0f, -0.5f), // 上
        float2(0.5f, 0.5f), // 右下
        float2(-0.5f, 0.5f) // 左下
    };

    float isOutside = 0.0f;

    [unroll]
    for (int i = 0; i < 3; i++)
    {
        float2 edge = v[(i + 1) % 3] - v[i];
        float2 toPoint = p - v[i];
        
        // 外積計算
        float cro = edge.x * toPoint.y - edge.y * toPoint.x;
        
        // 時計回りの定義で、cro > 0 なら外側 (左手系/右利き座標系によるが通常はこれ)
        // 判定が逆なら step(cro, 0.0f) に入れ替えてください
        isOutside += step(cro, 0.0f);
    }

    // 1つでも外側にあれば isOutside > 0 になるため、飽和させて 0 か 1 にする
    float mask = 1.0f - saturate(isOutside);

    return color * float4(1.0f, 1.0f, 1.0f, mask);
}


float4 main(PS_INPUT input) : SV_Target0
{
    
    // 1. アスペクト比 (横 / 縦)
    float aspect = windowSize.x / windowSize.y;
    
    float outputShapeID = input.ShapeID; // 0: 四角形, 1: 円形, 2: 三角形
    
    return (outputShapeID < 0.5f) ?
        getRectangle(input.Tex, aspect, input.Instance_Color) :
        (outputShapeID < 1.5f) ?
            getCircle(input.Tex, aspect, input.Instance_Color) :

    getTriangle(input.Tex, aspect, input.Instance_Color);
}
