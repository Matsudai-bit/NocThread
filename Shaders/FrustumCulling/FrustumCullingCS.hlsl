
// フラスタム平面情報（全スレッド共通）
cbuffer CameraFrustum : register(b0)
{
    float4 FrustumPlanes[6];
    int maxIndexCount;
};

// CSの入力情報（球情報）
struct SphereCompute
{
    float radius;
    float3 position;
};

struct OutCompute
{
    int index ;
};
// CS設定
struct CSInput
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group : SV_GroupID;
    uint groupIndex : SV_GroupIndex;
    uint3 dispatch : SV_DispatchThreadID;
};

// In
StructuredBuffer<SphereCompute> particle : register(t0);
// Out
RWStructuredBuffer<OutCompute> BufOut : register(u0);

#define size_x    256
#define size_y      1
#define size_z      1

[numthreads(size_x, size_y, size_z)]
void main(const CSInput input)
{
    int index = input.dispatch.x;
    
    // 境界チェック（ここはメモリ破壊防止のため return が最も安全）
    if (index >= maxIndexCount)
        return;

    SphereCompute p = particle[index];

    // 全ての平面に対して「内側かどうか」を 1.0 (True) か 0.0 (False) で累積
    float isVisible = 1.0f;

    [unroll] // ループを物理的に展開して分岐を消去するようコンパイラに指示
    for (int i = 0; i < 6; ++i)
    {
        float dist = dot(FrustumPlanes[i].xyz, p.position) + FrustumPlanes[i].w;
        
        // step(a, b) は a <= b なら 1.0、そうでなければ 0.0 を返す
        // 外向き法線の場合、dist <= p.radius なら合格
        isVisible *= step(dist, p.radius);
    }

    // --- 2. 書き込み時の分岐排除 ---
    
    // isVisible が 1.0 なら index、0.0 なら -1 を書き込む
    BufOut[index].index = (isVisible > 0.5f) ? (int) index : -1;
  
}