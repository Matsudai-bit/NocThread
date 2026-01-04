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

#include "Game/GameObjects/CheckPoint/CheckpointObject/CheckpointObject.h"

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
	m_collider = std::make_unique<AABB>(GetTransform()->GetPosition(), SimpleMath::Vector3(7.0f, 5.0f, 7.0f));

	m_checkpointObject = std::make_unique<CheckpointObjectController>();

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

	pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get()));

	// チェックポイントを有効化する
	m_isEnabled = true;


	// スケールの設定
	m_checkpointObject->SetScale(2.0f);


	m_checkpointObject->Initialize(GetCommonResources()->GetResourceManager());

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
	m_checkpointObject->SetPosition(GetTransform()->GetPosition());
	m_collider->SetCenter(GetTransform()->GetPosition());

	m_checkpointObject->Update(deltaTime);
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
	

	m_checkpointObject->Draw(context, GetCommonResources()->GetCommonStates(), camera);

	if (m_isEnabled)
	{
		// 目印
		auto cylinder = DirectX::GeometricPrimitive::CreateCylinder(context, 1000.f, 0.4f);


		Matrix transMat = Matrix::CreateTranslation(GetTransform()->GetPosition());
		Matrix rotateMat = Matrix::CreateFromQuaternion(GetTransform()->GetRotation());
		Matrix scaleMat = Matrix::CreateScale(GetTransform()->GetScale());

		Matrix world = scaleMat * rotateMat * transMat;
		world = SimpleMath::Matrix::Identity;
		world *= Matrix::CreateTranslation(GetTransform()->GetPosition());
		cylinder->Draw(world, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::GreenYellow);

		//m_collider->Draw(context, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	}

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

		m_checkpointObject->RequestLookAtHelicopter();
	}
}
