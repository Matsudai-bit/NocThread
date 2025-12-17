/*****************************************************************//**
* @file    Checkpoint.cpp
* @brief   ゲームのチェックポイントに関するソースファイル
*
* @author  松下大暉
* @date    2025/2/17
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Checkpoint.h"

// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"
// リソース関連
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
// 描画関連
#include "Game/Common/Camera/Camera.h"
// ゲームシステム関連
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
// イベント関連
#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Checkpoint::Checkpoint()
	: m_isEnabled{ false }
{
	// コライダーの作成
	//m_collider = std::make_unique<Cylinder>(SimpleMath::Vector3::UnitY, 1.0f, GetTransform()->GetPosition(), 3.0f);
	//m_collider = std::make_unique<Sphere>(GetTransform()->GetPosition(), 1.0f);
	m_collider = std::make_unique<AABB>(GetTransform()->GetPosition(), SimpleMath::Vector3(9.0f, 3.0f, 9.0f));
}



/**
 * @brief デストラクタ
 */
Checkpoint::~Checkpoint()
{

}


/**
 * @brief 初期化処理
 * 
 * @param[in] pCommonResources　共通リソース
 * @param[in] pCollisionManager 衝突管理
 */
void Checkpoint::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	auto context = pCommonResources->GetDeviceResources()->GetD3DDeviceContext();
	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	pCollisionManager->AddCollisionObjectData(this, m_collider.get());

	// チェックポイントを有効化する
	m_isEnabled = true;
	// モデルの作成
	m_model = GeometricPrimitive::CreateTeapot(context, 1.0f);

	// スケールの作成
	GetTransform()->SetScale(2.0f);



}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void Checkpoint::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	m_collider->SetCenter(GetTransform()->GetPosition());
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return camera カメラ
 */
void Checkpoint::Draw(const Camera& camera)
{
	using namespace SimpleMath;

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();


	Matrix transMat		= Matrix::CreateTranslation(GetTransform()->GetPosition());
	Matrix rotateMat	= Matrix::CreateFromQuaternion(GetTransform()->GetRotation());
	Matrix scaleMat		= Matrix::CreateScale(GetTransform()->GetScale());

	Matrix world = scaleMat * rotateMat * transMat;

	m_model->Draw(world, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	// 目印
	auto cylinder = DirectX::GeometricPrimitive::CreateCylinder(context, 1000.f, 0.5f);

	world = SimpleMath::Matrix::Identity;
	world *= Matrix::CreateTranslation(GetTransform()->GetPosition());
	cylinder->Draw(world, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::GreenYellow);

	m_collider->Draw(context, camera.GetViewMatrix(), camera.GetProjectionMatrix());

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Checkpoint::Finalize()
{

}

/**
 * @brief 衝突処理
 * 
 * @param[in] info　衝突情報
 */
void Checkpoint::OnCollision(const CollisionInfo& info)
{
	if (info.pOtherObject->GetTag() == GameObjectTag::PLAYER && m_isEnabled)
	{
		m_isEnabled = false;
		// プレイヤーがチェックポイントを通過したことを通知する
		GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::CHECKPOINT_PASSED);

	}
}
