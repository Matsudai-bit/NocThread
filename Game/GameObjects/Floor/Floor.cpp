/*****************************************************************//**
 * @file    Floor.h
 * @brief   足場に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/26
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Floor.h"

// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"

// データベース関連
#include "Game/Common/Database/TextureDatabase.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"

// グラフィック関連
#include "Game/Common/Camera/Camera.h"


using namespace DirectX;



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Floor::Floor()
    : m_collider{}
{

}



/**
 * @brief デストラクタ
 */
Floor::~Floor()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Floor::Initialize(const DirectX::SimpleMath::Vector3& pos, const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
    // 共通リソースの設定
    SetCommonResources(pCommonResources);

    auto device     = pCommonResources->GetDeviceResources()->GetD3DDevice();
    auto context    = pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	m_texture = GetCommonResources()->GetResourceManager()->CreateTexture(TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::TEXTURE_FLOOR));

    float halfWidth = WIDTH / 2.0f;

    // 中心座標の設定
    GetTransform()->SetPosition(pos);

    // 各座標の算出
    m_parentVertexes[0].position = GetTransform()->GetPosition() + DirectX::SimpleMath::Vector3( halfWidth, 0.0f, -halfWidth);
    m_parentVertexes[1].position = GetTransform()->GetPosition() + DirectX::SimpleMath::Vector3(-halfWidth, 0.0f, -halfWidth);
    m_parentVertexes[2].position = GetTransform()->GetPosition() + DirectX::SimpleMath::Vector3(-halfWidth, 0.0f,  halfWidth);
    m_parentVertexes[3].position = GetTransform()->GetPosition() + DirectX::SimpleMath::Vector3( halfWidth, 0.0f,  halfWidth);

    //m_parentVertexes[0].position = GetPosition() + DirectX::SimpleMath::Vector3(-halfWidth, 0.0f, -halfWidth);
    //m_parentVertexes[1].position = GetPosition() + DirectX::SimpleMath::Vector3( halfWidth, 0.0f, -halfWidth);
    //m_parentVertexes[2].position = GetPosition() + DirectX::SimpleMath::Vector3(halfWidth, 0.0f, halfWidth);
    //m_parentVertexes[3].position = GetPosition() + DirectX::SimpleMath::Vector3(halfWidth, 0.0f, halfWidth);

    // コライダーの生成
    m_collider = std::make_unique<AABB>(GetTransform()->GetPosition() - SimpleMath::Vector3(0.0f, 1.0f, 0.0f) * (WIDTH / 2.0f), SimpleMath::Vector3(WIDTH, WIDTH , WIDTH));

    for (int i = 0; i < VERTEX_NUM; i++)
    {
        m_vertexes[i] = m_parentVertexes[i];
    }

    // 衝突判定管理への登録
    pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get()));

    GetTransform()->SetPosition(SimpleMath::Vector3::Zero);

    m_effect = std::make_unique<BasicEffect>(device);
    // ライト(ON)
    m_effect->SetLightingEnabled(false);
    // 頂点カラー(OFF)
    m_effect->SetVertexColorEnabled(false);
    // テクスチャ(ON)
    m_effect->SetTextureEnabled(true);

    // 入力レイアウト作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionNormalTexture>(
            device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );


    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionNormalTexture>>(context);

}


/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool Floor::UpdateTask(float deltaTime)
{
    deltaTime;
    return true;
}



/**
 * @brief 描画処理
 *
 * @param[in] camera カメラ
 *
 * @return なし
 */
void Floor::DrawTask(const Camera& camera)
{

    auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
    CommonStates* pStates = GetCommonResources()->GetCommonStates();

    // 深度ステンシルバッファの設定
    context->OMSetDepthStencilState(pStates->DepthDefault(), 0);

    // ブレンドステートの設定
    context->OMSetBlendState(pStates->AlphaBlend(), nullptr, 0xffffffff);

    // カリングの設定
    //context->RSSetState(pStates->CullNone());              // カリングしない
    context->RSSetState(pStates->CullClockwise());              // カリングしない


    // テクスチャサンプラーの設定
    ID3D11SamplerState* samplers[] = { pStates->LinearWrap()};
    context->PSSetSamplers(0, 1, samplers);

    SimpleMath::Matrix rotationZ = SimpleMath::Matrix::CreateRotationZ(m_rotation.z);
    SimpleMath::Matrix rotationX = SimpleMath::Matrix::CreateRotationX(m_rotation.x);

    // ワールド行列
    SimpleMath::Matrix world = /*rotationZ * rotationX*/ SimpleMath::Matrix::Identity;
    m_effect->SetWorld(world);
    // ビュー行列
    m_effect->SetView(camera.GetViewMatrix());
    // 射影行列
    m_effect->SetProjection(camera.GetProjectionMatrix());

    // テクスチャ
    m_effect->SetTexture(m_texture.Get());

    // エフェクトを適応する
    m_effect->Apply(context);

    // 入力レイアウト
    context->IASetInputLayout(m_inputLayout.Get());

    

    // プリミティブバッチの描画
    m_primitiveBatch->Begin();

    // 四角形の描画
    m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, INDEXES, 6, m_vertexes, 4);

    m_primitiveBatch->End();

    //m_collider->Draw(context,view, proj);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Floor::Finalize()
{

}

/**
 * @brief 回転する
 * 
 * @param[in] rotation　回転
 */
void Floor::Rotate(const DirectX::SimpleMath::Vector3& rotation)
{
    if (rotation.LengthSquared() <= 0.0f)return;

    m_rotation = rotation;


    // 回転行列の作成
    SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateFromYawPitchRoll(rotation);

    // **** 各座標を回転させる ****
    m_vertexes[0].position = SimpleMath::Vector3::TransformNormal(m_parentVertexes[0].position, rotationMatrix);
    m_vertexes[1].position = SimpleMath::Vector3::TransformNormal(m_parentVertexes[1].position, rotationMatrix);
    m_vertexes[2].position = SimpleMath::Vector3::TransformNormal(m_parentVertexes[2].position, rotationMatrix);
    m_vertexes[3].position = SimpleMath::Vector3::TransformNormal(m_parentVertexes[3].position, rotationMatrix);


    //// コライダーを回転させる
    //m_collider->Rotate(m_rotation);
}

void Floor::PreCollision()
{
}

/**
 * @brief 衝突処理
 * 
 * @param[in] info 衝突情報
 */
void Floor::OnCollision(const CollisionInfo& info)
{
    UNREFERENCED_PARAMETER(info);

  
}

/**
 * @brief 回転の取得
 * 
 * @return 回転
 */
DirectX::SimpleMath::Vector3 Floor::GetRotate()
{
    return m_rotation;
}


