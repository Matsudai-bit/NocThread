/*****************************************************************//**
 * @file    EscapeHelicopter.h
 * @brief   脱出用ヘリコプターに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/03
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EscapeHelicopter.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/Common/ResourceManager/ResourceManager.h"

#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Camera/Camera.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
EscapeHelicopter::EscapeHelicopter()
{

}



/**
 * @brief デストラクタ
 */
EscapeHelicopter::~EscapeHelicopter()
{

}




/**
 * @brief 初期化処理
 * 
 * @param[in] pCommonResources	共通リソース
 * @param[in] pCollisionManager	衝突管理
 */
void EscapeHelicopter::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	SetScale(0.8f);

	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	// ヘリコプターモデルの作成
	m_helicopterModel = pCommonResources->GetResourceManager()->CreateModel("Helicopter.sdkmesh");

	// コライダの作成
	m_collider = std::make_unique<AABB>(GetPosition(),SimpleMath::Vector3( SimpleMath::Vector3(18.0f, 10.0f, 10.0f) * GetScale() ));

	// 衝突管理へ登録
	pCollisionManager->AddCollisionObjectData(this, m_collider.get());
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void EscapeHelicopter::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
	m_collider->SetCenter(GetPosition());
}



/**
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 */
void EscapeHelicopter::Draw(const Camera& camera)
{
	using namespace SimpleMath;

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	// ワールド行列の作成
	Matrix world = Matrix::Identity;

	world *= Matrix::CreateScale(GetScale());
	world *= Matrix::CreateFromQuaternion(GetRotate());
	world *= Matrix::CreateTranslation(GetPosition());

	// モデルの描画
	m_helicopterModel.Draw(
		GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext(),
		*GetCommonResources()->GetCommonStates(),
		world,
		camera.GetViewMatrix(),
		camera.GetProjectionMatrix());

	// 目印
	auto cylinder = DirectX::GeometricPrimitive::CreateCylinder(context, 1000.f, 0.5f);

	world = SimpleMath::Matrix::Identity;
	world *= Matrix::CreateTranslation(GetPosition());
	cylinder->Draw(world, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::Purple);


	// コライダーの表示
	//m_collider->Draw(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext(), view, proj);
	
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EscapeHelicopter::Finalize()
{

}

/**
 * @brief 衝突時に呼ばれる
 * 
 * @param[in] pHitObject   衝突オブジェクト
 * @param[in] pHitCollider	衝突コライダー
 */
void EscapeHelicopter::OnCollision(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);
	UNREFERENCED_PARAMETER(pHitCollider);

	if (pHitObject->GetTag() == GameObjectTag::PLAYER)
	{
		GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::ESCAPE_SUCCESS);
	}
}
