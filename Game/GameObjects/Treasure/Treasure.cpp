/*****************************************************************//**
 * @file    Treasure.h
 * @brief   お宝クラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Treasure.h"

#include "Game/Common/DeviceResources.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"

#include "Game/Common/GameEffect/GameEffectManager.h"
#include "Game/Common/GameEffect/Effects/SimpleParticle/SimpleParticle.h"
#include "Game/Common/Camera/MainCamera/MainCamera.h"
#include "Game/Common/GameEffect/GameEffectController.h"

#include "Game/Common/Camera/Camera.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Treasure::Treasure()
	: m_isActive{ true }
	, m_effectId{ 0 }
{

}



/**
 * @brief デストラクタ
 */
Treasure::~Treasure()
{

}



/**
 * @brief 初期化処理
 * 
 * @param[in] pCommonResources　　	共通リソース
 * @param[in] pCollisionManager		衝突管理
 */
void Treasure::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	SetScale(2.5f);

	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	// モデルの設定
	m_model = GetCommonResources()->GetResourceManager()->CreateModel("treasure.sdkmesh");

	// コライダーの設定
	m_collider = std::make_unique<Sphere>(GetPosition(), 1.0f * GetScale());

	// 衝突管理の登録
	pCollisionManager->AddCollisionObjectData(this, m_collider.get());

	m_isActive = true;

	GameEffectManager::EffectClip clip;
	clip.isLoop = true;
	auto simpleParticle = std::make_unique<SimpleParticle>(GetCommonResources()->GetDeviceResources(), GetPosition(), MainCamera::GetInstance()->GetCamera());
	m_effectId	=	GameEffectController::GetInstance()->PlayEffect(std::move(simpleParticle), clip);

	
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void Treasure::Update(float deltaTime)
{
	SetRotate(GetRotate() * SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::Up, XMConvertToRadians(180.0f * deltaTime)));
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Treasure::Draw(const Camera& camera)
{
	if (m_isActive == false) { return; }

	using namespace SimpleMath;

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetCommonResources()->GetCommonStates();

	// **** モデルの描画処理 ****
	Matrix world = Matrix::Identity;

	world *= Matrix::CreateScale(GetScale());
	world *= Matrix::CreateFromQuaternion(GetRotate());
	world *= Matrix::CreateTranslation(GetPosition());

	m_model.UpdateEffects(
		[](IEffect* effect)
		{
			auto basicEffect = dynamic_cast<BasicEffect*>(effect);

			if (basicEffect)
			{
				basicEffect->SetLightingEnabled(true);

				basicEffect->SetLightEnabled(0, true);
				basicEffect->SetLightEnabled(1, true);
				basicEffect->SetLightEnabled(2, true);
			}
		}
	);

	m_model.Draw(context, *states, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	// 目印
	auto cylinder = DirectX::GeometricPrimitive::CreateCylinder(context, 1000.f, 0.5f);

	world = SimpleMath::Matrix::Identity;
	world *= Matrix::CreateTranslation(GetPosition());
	cylinder->Draw(world, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::GreenYellow);

	//m_collider->Draw(context, view, proj);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Treasure::Finalize()
{

}


/**
 * @brief 衝突処理
 * 
 * @param[in] pHitObject	衝突オブジェクト
 * @param[in] pHitCollider	衝突コライダー
 */
void Treasure::OnCollision(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);
	UNREFERENCED_PARAMETER(pHitCollider);

	switch (pHitObject->GetTag())
	{
	case GameObjectTag::PLAYER:
		m_isActive = false;
		GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::STOLE_TREASURE);
		// エフェクトの停止
		GameEffectController::GetInstance()->StopEffect(m_effectId);
		break;
	default:
		break;

	}
}
