/*****************************************************************//**
 * @file    StageObject.h
 * @brief   ステージオブジェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageObject.h"
#include <typeindex>

// ゲームオブジェクト関連
#include "Game/GameObjects/StageObject/State/IdleStageObjectState.h"

// 状態関連
#include "Game/GameObjects/StageObject/State/ThrownStageObjectState/ThrownStageObjectState.h"

// フレームワーク関連
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/Event/WireSystemObserver/WireEventData.h"

// グラフィック関連
#include "Game/Common/Camera/Camera.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
StageObject::StageObject()
	: m_isActive{ true }
{

}



/**
 * @brief デストラクタ
 */
StageObject::~StageObject()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageObject::Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	RemoveAllEventListener();
	RemoveAllQueuedEvents();

	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	// キャスト登録
	RegisterChildType(this);

	RegisterChildType<IWireEventObserver>(this);

	GetTransform()->SetScale(0.4f);

	// コライダの作成
	m_collider = std::make_unique<Sphere>(GetTransform()->GetPosition(), GetTransform()->GetScale().x);

	pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get()));

	// モデルの取得
	m_model = GetCommonResources()->GetResourceManager()->CreateModel(STAGE_OBJECT_MODEL_FILE_NAME);


	m_isActive = true;

	// ステートマシーンの作成
	m_stateMachine = std::make_unique<StateMachine<StageObject>>(this);

	// 初期ステートの変更
	m_stateMachine->ChangeState<IdleStageObjectState>();


}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void StageObject::Update(float deltaTime)
{
	if (m_isActive == false) return;
	ApplyEvents();
	// ステートマシーンの更新処理
	m_stateMachine->Update(deltaTime);



}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageObject::Draw(const Camera& camera)
{
	if (m_isActive == false) return;


	using namespace SimpleMath;

	// ステートの描画処理
	m_stateMachine->Draw();

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();

	Matrix transform = Matrix::CreateTranslation(GetTransform()->GetPosition());
	Matrix rotate = Matrix::CreateFromQuaternion(GetTransform()->GetRotation());
	Matrix scale = Matrix::CreateScale(GetTransform()->GetScale());

	Matrix world = scale * rotate * transform;


	m_model.Draw(context, *GetCommonResources()->GetCommonStates(), world,  camera.GetViewMatrix(), camera.GetProjectionMatrix());

}

/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageObject::Finalize()
{
	RemoveAllEventListener();
	RemoveAllQueuedEvents();
}

/**
 * @brief 物理的な移動
 *
 * @param[in] deltaTime
 */
void StageObject::ApplyPhysic(const float& deltaTime)
{
	// 重力を加える
	ApplyGravity(deltaTime);



}

/**
 * @brief 摩擦を加える
 */
void StageObject::ApplyFriction()
{
	// 摩擦を加える
	// 後に地面に着いている間にいつか変える
	SimpleMath::Vector3 velocity = GetVelocity();
	velocity = velocity * SimpleMath::Vector3(FRICTION, 1.0f, FRICTION);

	velocity = (0.01f > velocity.Length()) ? SimpleMath::Vector3::Zero : velocity;
	SetVelocity(velocity);
}
/**
 * @brief 重力を適用
 *
 * @param[in] deltaTime　経過時間
 */
void StageObject::ApplyGravity(const float& deltaTime)
{

	// 重力を加える
	SimpleMath::Vector3 velocity = GetVelocity();
	velocity += GRAVITY_ACCELERATION * deltaTime;

	SetVelocity(velocity);
}

/**
 * @brief 移動の適用
 *
 * @param[in] deltaTime　経過時間
 */
void StageObject::Move(const float& deltaTime)
{

	SimpleMath::Quaternion rotate = GetTransform()->GetRotation();


	// **** 座標の更新 ****
	// 座標の算出
	SimpleMath::Vector3 position = GetTransform()->GetPosition() + GetVelocity() * deltaTime;
	GetTransform()->SetPosition(position);

	// コライダの更新処理
	m_collider->Transform(position);
}

/**
 * @brief 掴む通知を受けたときの処理
 *
 * @param[in] eventData イベントデータ
 */
void StageObject::OnWireGrab(const WireEventData& eventData)
{
	UNREFERENCED_PARAMETER(eventData);

	
}

/**
 * @brief 離す通知を受けたときの処理
 *
 * @param[in] eventData イベントデータ
 */
void StageObject::OnWireRelease(const WireEventData& eventData)
{
	UNREFERENCED_PARAMETER(eventData);

}

/**
 * @brief 引く通知を受けたときの処理
 *
 * @param[in] eventData イベントデータ
 */
void StageObject::OnWirePull(const WireEventData& eventData)
{
	UNREFERENCED_PARAMETER(eventData);

}

/**
 * @brief 投げる通知を受けたときの処理
 *
 * @param[in] eventData イベントデータ
 */
void StageObject::OnWireThrow(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse)
{
	UNREFERENCED_PARAMETER(eventData);


	AddForceToVelocity(throwImpulse);

	m_stateMachine->ChangeState<ThrownStageObjectState>();
}

/**
 * @brief 掴んでいる状態の通知を受けたときの処理
 *
 * @param[in] eventData イベントデータ
 */
void StageObject::OnWireHover(const WireEventData& eventData)
{
	using namespace SimpleMath;

	GetTransform()->SetPosition(eventData.grabPos);
}

/**
 * @brief 衝突処理
 *
 * @param[in] info 衝突情報
 */
void StageObject::OnCollision(const CollisionInfo& info)
{
	if (info.pOtherObject->GetTag() == GameObjectTag::FLOOR || info.pOtherObject->GetTag() == GameObjectTag::WALL)
	{
		if (const Box2D* box = dynamic_cast<const Box2D*>(info.pOtherCollider))
		{
			Plane plane = box->GetPlane();

			SimpleMath::Vector3 overlap = CalcOverlap(plane, *m_collider);

			SimpleMath::Vector3 position = GetTransform()->GetPosition() + overlap;

			if (overlap.y > 0.0f)
			{
				SimpleMath::Vector3 velocity = GetVelocity() * SimpleMath::Vector3(1.0f, 0.0f, 1.0f);
				SetVelocity(velocity);

				// 摩擦を加える
				ApplyFriction();
			}

			GetTransform()->SetPosition(position);
			m_collider->Transform(position);
		}
	}

	if (info.pOtherObject->GetTag() == GameObjectTag::STAGE_OBJECT)
	{
	
			info.pOtherObject->FireEvent(GameObjectEventType::THROW_HIT, std::make_unique<ThrowHitEventData>(this, m_collider.get()));
		
	}
}
