/*****************************************************************//**
 * @file    Wall.h
 * @brief   足場に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/26
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Wall.h"


// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"



using namespace DirectX;



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Wall::Wall()
    : m_collider{}
{

}



/**
 * @brief デストラクタ
 */
Wall::~Wall()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Wall::Initialize(
    const SimpleMath::Vector3& pos,
    const PlaceDirection& placeDirection,
    const WallData& wallData,
    CommonResources* pCommonResources, 
    CollisionManager*   pCollisionManager,
    std::string fileName,
    float height)
{
    // 共通リソースの設定
    SetCommonResources(pCommonResources);

    auto context    = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
    auto device     = GetCommonResources()->GetDeviceResources()->GetD3DDevice();

    m_height = height;
    
    // 中心座標の設定
    GetTransform()->SetPosition(pos);

    // 壁を置く方向を設定
    PutPlaceDirection(placeDirection);

    for (int i = 0; i < VERTEX_NUM; i++)
    {
        m_vertexes[i] = m_parentVertexes[i];
    }


    m_effect = std::make_unique<BasicEffect>(device);
    // ライト(ON)
    m_effect->SetLightingEnabled(false);
    // 頂点カラー(OFF)
    m_effect->SetVertexColorEnabled(false);
    // テクスチャ(ON)
    m_effect->SetTextureEnabled(true);

    // テクスチャの取得
   // m_texture = GetCommonResources()->GetResourceManager()->CreateTexture("wall.dds");
    m_texture = GetCommonResources()->GetResourceManager()->CreateTexture(fileName);

    // 入力レイアウト作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionNormalTexture>(
            device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );



    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionNormalTexture>>(context);

    // コライダーの設定
    // 前面
    if (wallData.facadeFace)
        m_collider.push_back(std::make_unique<Box2D>(m_vertexes[0].position, m_vertexes[1].position, m_vertexes[2].position, m_vertexes[3].position, pCommonResources->GetDeviceResources()));
    // 後面
    if (wallData.backFace)
        m_collider.push_back(std::make_unique<Box2D>(m_vertexes[4].position, m_vertexes[5].position, m_vertexes[6].position, m_vertexes[7].position, pCommonResources->GetDeviceResources()));
    // 左面
    if (wallData.leftFace)
         m_collider.push_back(std::make_unique<Box2D>(m_vertexes[8].position, m_vertexes[9].position, m_vertexes[10].position, m_vertexes[11].position, pCommonResources->GetDeviceResources()));
    // 右面
    if (wallData.rightFace)
        m_collider.push_back(std::make_unique<Box2D>(m_vertexes[12].position, m_vertexes[13].position, m_vertexes[14].position, m_vertexes[15].position, pCommonResources->GetDeviceResources()));
    // 上面
    m_collider.push_back(std::make_unique<Box2D>(m_vertexes[16].position, m_vertexes[17].position, m_vertexes[18].position, m_vertexes[19].position, pCommonResources->GetDeviceResources()));
    // 底面
    m_collider.push_back(std::make_unique<Box2D>(m_vertexes[20].position, m_vertexes[21].position, m_vertexes[22].position, m_vertexes[23].position, pCommonResources->GetDeviceResources()));
    //m_collider[5].Set(m_vertexes[20].position, m_vertexes[21].position, m_vertexes[22].position, m_vertexes[23].position);

        // 衝突判定管理への登録
    for (auto& collider : m_collider)
    {
        pCollisionManager->AddCollisionData(CollisionData(this, collider.get()));
    }
 
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void Wall::Update(float deltaTime)
{
    deltaTime;

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Wall::Draw(
    SimpleMath::Matrix* pView, SimpleMath::Matrix* pProj)
{
    auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
    auto pStates = GetCommonResources()->GetCommonStates();



    // 深度ステンシルバッファの設定
    context->OMSetDepthStencilState(pStates->DepthDefault(), 0);

    // ブレンドステートの設定
    context->OMSetBlendState(pStates->AlphaBlend(), nullptr, 0xffffffff);

    // カリングの設定
    context->RSSetState(pStates->CullClockwise());              // カリングしない


    // テクスチャサンプラーの設定
    ID3D11SamplerState* samplers[] = { pStates->PointWrap() };
    context->PSSetSamplers(0, 1, samplers);

    SimpleMath::Matrix rotationZ = SimpleMath::Matrix::CreateRotationZ(m_rotation.z);
    SimpleMath::Matrix rotationX = SimpleMath::Matrix::CreateRotationX(m_rotation.x);

    // ワールド行列
    SimpleMath::Matrix world = SimpleMath::Matrix::Identity;/* rotationZ* rotationX;*/
    m_effect->SetWorld(world);
    // ビュー行列
    m_effect->SetView(*pView);
    // 射影行列
    m_effect->SetProjection(*pProj);

    // テクスチャ
    m_effect->SetTexture(m_texture.Get());


    // エフェクトを適応する
    m_effect->Apply(context);

    // 入力レイアウト
    context->IASetInputLayout(m_inputLayout.Get());




    // プリミティブバッチの描画
    m_primitiveBatch->Begin();

    // 四角形の描画
    m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, INDEXES, VERTEX_NUM, m_vertexes, static_cast<size_t>( 4 * 6));

    m_primitiveBatch->End();

    //// コライダーの描画
    //for (auto& collider : m_collider)
    //{
    //    collider->Draw(*pView, *pProj);
    //}
    

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Wall::Finalize()
{

}

/**
 * @brief 回転する
 * 
 * @param[in] rotation 回転度合い
 */
void Wall::Rotate(DirectX::SimpleMath::Vector3 rotation)
{
    if (rotation == SimpleMath::Vector3::Zero)  return;

    m_rotation = rotation;

    // 回転行列の作成
    SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateFromYawPitchRoll(m_rotation);

    // **** 各座標を回転させる ****
    for (int i = 0; i < VERTEX_NUM; i++)
    {
        m_vertexes[i].position = SimpleMath::Vector3::Transform(m_parentVertexes[i].position, rotationMatrix);

    }

    // コライダーの回転
    for (auto& collider : m_collider)
    {
        collider->Rotate(m_rotation);
    }
}

/**
 * @brief 衝突処理
 *
 * @param[in] info 衝突情報
 */
void Wall::OnCollision(const CollisionInfo& info)
{
    UNREFERENCED_PARAMETER(info);
}

/**
 * @brief 置く方向に設置する
 *
 * @param[in] placeDirection 置く方向
 */
void Wall::PutPlaceDirection(const PlaceDirection& placeDirection)
{

    float halfDeph = DEPH / 2.0f;
    float halfWidth = 0.49f + halfDeph;

    // 落ちないようにするために高さを高くする

    SimpleMath::Vector3 position = GetTransform()->GetPosition();

    switch (placeDirection)
    {
    case PlaceDirection::UP:
        // 各座標の算出

        // 前面
        m_parentVertexes[0].position = position + SimpleMath::Vector3(+halfWidth, m_height, -(DISTANCE - halfDeph));
        m_parentVertexes[1].position = position + SimpleMath::Vector3(-halfWidth, m_height, -(DISTANCE - halfDeph));
        m_parentVertexes[2].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, -(DISTANCE - halfDeph));
        m_parentVertexes[3].position = position + SimpleMath::Vector3(+halfWidth, 0.0f, -(DISTANCE - halfDeph));

        // 後面
        m_parentVertexes[4].position = position + SimpleMath::Vector3(-halfWidth, m_height, -(DISTANCE + halfDeph));
        m_parentVertexes[5].position = position + SimpleMath::Vector3(+halfWidth, m_height, -(DISTANCE + halfDeph));
        m_parentVertexes[6].position = position + SimpleMath::Vector3(+halfWidth, 0.0f, -(DISTANCE + halfDeph));
        m_parentVertexes[7].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, -(DISTANCE + halfDeph));

        // 左面
        m_parentVertexes[8].position = position + SimpleMath::Vector3(-halfWidth, m_height, -(DISTANCE - halfDeph));
        m_parentVertexes[9].position = position + SimpleMath::Vector3(-halfWidth, m_height, -(DISTANCE + halfDeph));
        m_parentVertexes[10].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, -(DISTANCE + halfDeph));
        m_parentVertexes[11].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, -(DISTANCE - halfDeph));

        // 右面
        m_parentVertexes[12].position = position + SimpleMath::Vector3(halfWidth, m_height, -(DISTANCE + halfDeph));
        m_parentVertexes[13].position = position + SimpleMath::Vector3(halfWidth, m_height, -(DISTANCE - halfDeph));
        m_parentVertexes[14].position = position + SimpleMath::Vector3(halfWidth, 0.0f, -(DISTANCE - halfDeph));
        m_parentVertexes[15].position = position + SimpleMath::Vector3(halfWidth, 0.0f, -(DISTANCE + halfDeph));

        // 上面
        m_parentVertexes[16].position = position + SimpleMath::Vector3(-halfWidth, m_height, -(DISTANCE - halfDeph));
        m_parentVertexes[17].position = position + SimpleMath::Vector3(halfWidth, m_height, -(DISTANCE - halfDeph));
        m_parentVertexes[18].position = position + SimpleMath::Vector3(halfWidth, m_height, -(DISTANCE + halfDeph));
        m_parentVertexes[19].position = position + SimpleMath::Vector3(-halfWidth, m_height, -(DISTANCE + halfDeph));

        // **底面 (追加)**
        m_parentVertexes[20].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, -(DISTANCE + halfDeph));
        m_parentVertexes[21].position = position + SimpleMath::Vector3(halfWidth, 0.0f, -(DISTANCE + halfDeph));
        m_parentVertexes[22].position = position + SimpleMath::Vector3(halfWidth, 0.0f, -(DISTANCE - halfDeph));
        m_parentVertexes[23].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, -(DISTANCE - halfDeph));

        break;
    case PlaceDirection::RIGHT:
        // 各座標の算出

          // 前面
        m_parentVertexes[0].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), m_height, halfWidth);
        m_parentVertexes[1].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), m_height, halfWidth);
        m_parentVertexes[2].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), 0.0f, halfWidth);
        m_parentVertexes[3].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), 0.0f, halfWidth);

        // 後面
        m_parentVertexes[4].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), m_height, -halfWidth);
        m_parentVertexes[5].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), m_height, -halfWidth);
        m_parentVertexes[6].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), 0.0f, -halfWidth);
        m_parentVertexes[7].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), 0.0f, -halfWidth);

        //左面
        m_parentVertexes[8].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), m_height, +halfWidth);
        m_parentVertexes[9].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), m_height, -halfWidth);
        m_parentVertexes[10].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), 0.0f, -halfWidth);
        m_parentVertexes[11].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), 0.0f, +halfWidth);

        // 右面
        m_parentVertexes[12].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), m_height, -halfWidth);
        m_parentVertexes[13].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), m_height, +halfWidth);
        m_parentVertexes[14].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), 0.0f, +halfWidth);
        m_parentVertexes[15].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), 0.0f, -halfWidth);

        // 上面
        m_parentVertexes[16].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), m_height, -halfWidth);
        m_parentVertexes[17].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), m_height, halfWidth);
        m_parentVertexes[18].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), m_height, halfWidth);
        m_parentVertexes[19].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), m_height, -halfWidth);

        // **底面 (追加)**
        m_parentVertexes[20].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), 0.0f, -halfWidth);
        m_parentVertexes[21].position = position + SimpleMath::Vector3((DISTANCE + halfDeph), 0.0f, halfWidth);
        m_parentVertexes[22].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), 0.0f, halfWidth);
        m_parentVertexes[23].position = position + SimpleMath::Vector3((DISTANCE - halfDeph), 0.0f, -halfWidth);

        break;
    case PlaceDirection::DOWN:
        // 各座標の算出

        // 前面
        m_parentVertexes[0].position = position + SimpleMath::Vector3(+halfWidth, m_height, (DISTANCE + halfDeph));
        m_parentVertexes[1].position = position + SimpleMath::Vector3(-halfWidth, m_height, (DISTANCE + halfDeph));
        m_parentVertexes[2].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, (DISTANCE + halfDeph));
        m_parentVertexes[3].position = position + SimpleMath::Vector3(+halfWidth, 0.0f, (DISTANCE + halfDeph));

        // 後面
        m_parentVertexes[4].position = position + SimpleMath::Vector3(-halfWidth, m_height, (DISTANCE - halfDeph));
        m_parentVertexes[5].position = position + SimpleMath::Vector3(+halfWidth, m_height, (DISTANCE - halfDeph));
        m_parentVertexes[6].position = position + SimpleMath::Vector3(+halfWidth, 0.0f, (DISTANCE - halfDeph));
        m_parentVertexes[7].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, (DISTANCE - halfDeph));

        // 左面
        m_parentVertexes[8].position = position + SimpleMath::Vector3(-halfWidth, m_height, (DISTANCE + halfDeph));
        m_parentVertexes[9].position = position + SimpleMath::Vector3(-halfWidth, m_height, (DISTANCE - halfDeph));
        m_parentVertexes[10].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, (DISTANCE - halfDeph));
        m_parentVertexes[11].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, (DISTANCE + halfDeph));

        // 右面
        m_parentVertexes[12].position = position + SimpleMath::Vector3(halfWidth, m_height, (DISTANCE - halfDeph));
        m_parentVertexes[13].position = position + SimpleMath::Vector3(halfWidth, m_height, (DISTANCE + halfDeph));
        m_parentVertexes[14].position = position + SimpleMath::Vector3(halfWidth, 0.0f, (DISTANCE + halfDeph));
        m_parentVertexes[15].position = position + SimpleMath::Vector3(halfWidth, 0.0f, (DISTANCE - halfDeph));

        // 上面
        m_parentVertexes[16].position = position + SimpleMath::Vector3(-halfWidth, m_height, (DISTANCE + halfDeph));
        m_parentVertexes[17].position = position + SimpleMath::Vector3(halfWidth, m_height, (DISTANCE + halfDeph));
        m_parentVertexes[18].position = position + SimpleMath::Vector3(halfWidth, m_height, (DISTANCE - halfDeph));
        m_parentVertexes[19].position = position + SimpleMath::Vector3(-halfWidth, m_height, (DISTANCE - halfDeph));

        // **底面 (追加)**
        m_parentVertexes[20].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, (DISTANCE - halfDeph));
        m_parentVertexes[21].position = position + SimpleMath::Vector3(halfWidth, 0.0f, (DISTANCE - halfDeph));
        m_parentVertexes[22].position = position + SimpleMath::Vector3(halfWidth, 0.0f, (DISTANCE + halfDeph));
        m_parentVertexes[23].position = position + SimpleMath::Vector3(-halfWidth, 0.0f, (DISTANCE + halfDeph));

        break;
    case PlaceDirection::LEFT:

        // 前面
        m_parentVertexes[0].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), m_height, halfWidth);
        m_parentVertexes[1].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), m_height, halfWidth);
        m_parentVertexes[2].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), 0.0f, halfWidth);
        m_parentVertexes[3].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), 0.0f, halfWidth);

        // 後面
        m_parentVertexes[4].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), m_height, -halfWidth);
        m_parentVertexes[5].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), m_height, -halfWidth);
        m_parentVertexes[6].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), 0.0f, -halfWidth);
        m_parentVertexes[7].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), 0.0f, -halfWidth);


        //左面
        m_parentVertexes[8].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), m_height, +halfWidth);
        m_parentVertexes[9].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), m_height, -halfWidth);
        m_parentVertexes[10].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), 0.0f, -halfWidth);
        m_parentVertexes[11].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), 0.0f, +halfWidth);

        // 右面
        m_parentVertexes[12].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), m_height, -halfWidth);
        m_parentVertexes[13].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), m_height, +halfWidth);
        m_parentVertexes[14].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), 0.0f, +halfWidth);
        m_parentVertexes[15].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), 0.0f, -halfWidth);

        // 上面
        m_parentVertexes[16].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), m_height, -halfWidth);
        m_parentVertexes[17].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), m_height, halfWidth);
        m_parentVertexes[18].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), m_height, halfWidth);
        m_parentVertexes[19].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), m_height, -halfWidth);

        // **底面 (追加)**
        m_parentVertexes[20].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), 0.0f, -halfWidth);
        m_parentVertexes[21].position = position + SimpleMath::Vector3(-(DISTANCE - halfDeph), 0.0f, halfWidth);
        m_parentVertexes[22].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), 0.0f, halfWidth);
        m_parentVertexes[23].position = position + SimpleMath::Vector3(-(DISTANCE + halfDeph), 0.0f, -halfWidth);

        break;

    default:
        break;
    }
}
