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

#include "Game/Common/Framework/GameObjectRegistry/GameObjectRegistry.h"
// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"

// フレームワーク関連
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"
// グラフィック関連
#include "Game/Common/Graphics/Camera/Camera.h"
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Graphics/GameEffect/Effects/SimpleParticle/SimpleParticle.h"
#include "Game/Common/Graphics/GameEffect/GameEffectController.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/CheckPoint/CheckpointObject/CheckpointObject.h"

// カメラ
using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Checkpoint::Checkpoint()
	: m_isEnabled{ false }
	, m_effectHandleID{ -1 }
	, m_canVisibleMark{ true }
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
void Checkpoint::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager, const DirectX::SimpleMath::Vector3& position)
{
	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get(), false));

	// チェックポイントを有効化する
	m_isEnabled = true;
	// マークを表示可能にする
	m_canVisibleMark = true;


	// トランスフォームの設定
	GetTransform()->SetPosition(position);

	// オブジェクトの初期化
	m_checkpointObject->GetTransform()->SetScale(2.0f);

	m_effectHandleID =  GameEffectController::GetInstance()->PlayEffect(std::make_unique<SimpleParticle>(GetCommonResources()->GetDeviceResources(), GetTransform()->GetPosition(), MainCamera::GetInstance()->GetCamera()), GameEffectManager::EffectClip(true));

	m_checkpointObject->Initialize(GetCommonResources()->GetResourceManager(), GetCommonResources()->GetDeviceResources()->GetD3DDevice());



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
	m_checkpointObject->GetTransform()->SetPosition(GetTransform()->GetPosition());
	m_collider->SetCenter(GetTransform()->GetPosition());

	m_checkpointObject->Update(deltaTime);

	// プレイヤーの取得
	auto player = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER);
	if (player)
	{
		// プレイヤーの距離が一定以下ならば目印を消す
		float distanceSqr = DirectX::SimpleMath::Vector3::DistanceSquared(player->GetTransform()->GetPosition(), GetTransform()->GetPosition());

		//if (distanceSqr <= CHECKPOINT_MARKER_HIDE_DISTANCE_SQR)
		m_canVisibleMark = (distanceSqr >= 100.0f * 100.0f);


	}

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

	if (m_isEnabled && m_canVisibleMark)
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

		m_checkpointObject->FireEvent(CheckpointControllEventID::LOOK_AT_HELICOPTER);

		if (m_effectHandleID != -1)
		{
			GameEffectController::GetInstance()->StopEffect(m_effectHandleID);
			m_effectHandleID = -1;
		}
	}
}
