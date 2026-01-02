

// パーティクル構造体
struct ParticleCompute
{
    float4 planes[6];

    float radius;
    float3 position;
};

struct ResultCompute
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
StructuredBuffer<ParticleCompute> particle : register(t0);
// Out
RWStructuredBuffer<ResultCompute> BufOut : register(u0);

#define size_x    256
#define size_y      1
#define size_z      1

[numthreads(size_x, size_y, size_z)]
void main(const CSInput input)
{
    uint index = input.dispatch.x;

    // バッファ外アクセスの防止
    // ※ 実際のパーティクル数を定数バッファ等で渡してチェックするのが安全です
    ParticleCompute p = particle[index];

    // 最初は「内側」と仮定する
    bool isVisible = true;

    // 全ての平面に対してチェック
   // 法線が「外向き」の場合のループ
    for (int i = 0; i < 6; ++i)
    {
        float dist = dot(p.planes[i].xyz, p.position) + p.planes[i].w;

    // 外側向きの場合、距離が +radius より大きければ「完全に外側」
        if (dist > p.radius)
        {
            isVisible = false;
            break;
        }
    }

    // 結果の書き込み
    if (isVisible)
    {
        BufOut[index].index = (int) index;
    }
    else
    {
        BufOut[index].index = -1;
    }
  
}