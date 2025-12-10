/*****************************************************************//**
 * @file    Enemy.h
 * @brief   敵に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/07/30
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Enemy.h"
#include "Game/Common/Helper/PhysicsHelper/PhysicsHelper.h"

#include "Game/GameObjects/Enemy/Behaviour/Update/IEnemyUpdateBehaviour.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Library/DirectXFramework/DeviceResources.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Game/Common/Event/WireSystemObserver/WireEventData.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Game/Common/Helper/MovementHelper/MovementHelper.h"
#include "Library/ImaseLib/DebugFont.h"

#include "Game/Common/ResultData/ResultData.h"

#include "Game/Common/Screen.h"
#include "Game/Common/Camera/Camera.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Enemy::Enemy()
	: m_isHold{false}
	, m_isThrow{false}
	, m_deltaTime{}
	, m_isActive{true}
	, m_isGrounded{false}
	
{

}



/**
 * @brief デストラクタ
 */
Enemy::~Enemy()
{

}

/**
 * @brief 初期化処理
 * 
 * @param[in] pCommonResources
 * @param[in] pCollisionManager
 */
void Enemy::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	UNREFERENCED_PARAMETER(pCommonResources);
	UNREFERENCED_PARAMETER(pCollisionManager);

	SetCommonResources(pCommonResources);

	m_model = pCommonResources->GetResourceManager()->CreateModel("security_guard_fly.sdkmesh");

	m_isHold = false;
	m_isThrow = false;
	// キャスト登録
	RegisterChildType(this);

	RegisterChildType<IWireEventObserver>(this);

	m_collider = std::make_unique<Sphere>(GetTransform()->GetPosition() + SimpleMath::Vector3(0.0f, 0.5f, 0.0f), 1.0f);

	pCollisionManager->AddCollisionObjectData(this,m_collider.get());

	m_deltaTime = 0.0f;

	// イベントの追加
	AddEventListener<ThrowHitEventData>(GameObjectEventType::THROW_HIT,
		[this](const ThrowHitEventData data)
		{
			m_isActive = false;
			ResultData::GetInstance()->UpperDestroyedNum();
		}
	);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void Enemy::Update(float deltaTime)
{
	ApplyEvents();
	if (m_isActive == false) { return; }

	if (m_isHold == false )
	{
		if (m_deltaTime <= 0.0f)
		{
			for (auto& behaviour : m_updateBehaviour)
			{
				behaviour->Update(this, deltaTime, GetCommonResources());
			}
		

			m_deltaTime = 0.0f;
			m_isThrow = false;
		}
		else
		{
			m_deltaTime -= deltaTime;
		}
	
	}


	
	SimpleMath::Vector3 velocity = GetVelocity();

	// 重力を加える
	velocity += GRAVITY_ACCELERATION * deltaTime;

	if (m_isGrounded)
	{
		// 摩擦を加える
		velocity = PhysicsHelper::CalculateFrictionVelocity(velocity, deltaTime, FRICTION, GameObject::GRAVITY_ACCELERATION.Length());
	}
	// 適用
	velocity = PhysicsHelper::CalculateDragVelocity(velocity, deltaTime, AIR_RESISTANCE);



	SetVelocity(velocity);

	SimpleMath::Vector3 position = MovementHelper::CalcPositionForVelocity(deltaTime, GetTransform()->GetPosition(), GetVelocity());
	GetTransform()->SetPosition(position );
	m_collider->Transform(GetTransform()->GetPosition() + SimpleMath::Vector3(0.0f, 0.5f, 0.0f));

	GetTransform()->SetRotation(MovementHelper::RotateForMoveDirection(deltaTime, GetTransform()->GetRotation(), GetTransform()->GetForward(), GetVelocity(), 0.1f));
}

void Enemy::Draw(const Camera& camera)
{
	if (m_isActive == false) { return; }
	using namespace SimpleMath;

	auto context	= GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states		= GetCommonResources()->GetCommonStates();

	Matrix world = Matrix::Identity;
	world *= Matrix::CreateFromQuaternion(GetTransform()->GetRotation());
	world *= Matrix::CreateTranslation(GetTransform()->GetPosition());

	m_model.Draw(context, *states, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	/*GetCommonResources()->GetDebugFont()->AddString(Screen::Get()->GetRight() - 600.0f, 90, Colors::White, L"position : %f, %f, %f ", GetPosition().x, GetPosition().y, GetPosition().z);
	GetCommonResources()->GetDebugFont()->AddString(Screen::Get()->GetRight() - 600.0f, 110, Colors::White, L"velocity : %f, %f, %f ", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	GetCommonResources()->GetDebugFont()->AddString(Screen::Get()->GetRight() - 600.0f, 130, Colors::White, L"speed : %f ", GetVelocity().Length());*/

	//m_collider->Draw(context, view, proj);
}






/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Enemy::Finalize()
{

}

/**
 * @brief 挙動の登録
 * 
 * @param[in] updateBehaviour 更新する時の挙動
 */
void Enemy::AddUpdateBehaviour(std::unique_ptr<IEnemyUpdateBehaviour> updateBehaviour)
{
	m_updateBehaviour.push_back(std::move(updateBehaviour));
}

/**
 * @brief 衝突管理
 *
 * @param[in] info 衝突情報
 */
void Enemy::OnCollision(const CollisionInfo& info)
{
 

	if (m_isThrow && info.pOtherObject->GetTag() == GameObjectTag::ENEMY)
	{
		info.pOtherObject->FireEvent(GameObjectEventType::THROW_HIT, std::make_unique<ThrowHitEventData>(this, m_collider.get()));
	}

	if (!m_isThrow && !m_isHold && info.pOtherObject->GetTag() == GameObjectTag::PLAYER)
	{
		info.pOtherObject->FireEvent(GameObjectEventType::CAUGHT, std::make_unique<CaughtEventData>(this));
	}

	if (info.pOtherObject->GetTag() == GameObjectTag::BUILDING || info.pOtherObject->GetTag() == GameObjectTag::FLOOR)
	{
		OnCollisionWithBuilding(info.pOtherObject, info.pOtherCollider);
	}

}

void Enemy::PreCollision()
{
	m_isGrounded = false;
}

void Enemy::PostCollision()
{
	if (std::abs(m_overlapTotal.LengthSquared()) > 0.0f)
	{

		SimpleMath::Vector3 overlap = m_overlapTotal;

		SimpleMath::Vector3 velocity = GetVelocity();

		// **** 押し出しとバウンド ****
		GetTransform()->SetPosition(PhysicsHelper::PushOut(overlap, GetTransform()->GetPosition()));

		SetVelocity(PhysicsHelper::ResolveBounce(overlap, 0.0f, velocity));

		m_overlapTotal = SimpleMath::Vector3::Zero;

		if (overlap.y > 0.0f)
		{
			m_isGrounded = true;
		}
	}
}

/**
 * @brief ワイヤーに掴まれた時の挙動
 * 
 * @param[in] eventData
 */
void Enemy::OnWireGrab(const WireEventData& eventData)
{
	GetTransform()->SetPosition(eventData.grabPos);
	ResetVelocity();

	m_isHold = true;
}

/**
 * @brief ワイヤーに掴まれている時
 *
 * @param[in] eventData
 */
void Enemy::OnWireHover(const WireEventData& eventData)
{
	GetTransform()->SetPosition(eventData.grabPos);
	ResetVelocity();
}

/**
 * @brief ワイヤーから離れた時
 * 
 * @param[in] eventData
 */
void Enemy::OnWireRelease(const WireEventData& eventData)
{
	UNREFERENCED_PARAMETER(eventData);
}

/**
 * @brief ワイヤーで引き寄せられる時
 * 
 * @param[in] eventData
 */
void Enemy::OnWirePull(const WireEventData& eventData)
{
	UNREFERENCED_PARAMETER(eventData);
}

/**
 * @brief ワイヤーでなげられるとき　
 * 
 * @param[in] eventData
 * @param[in] throwImpulse
 */
void Enemy::OnWireThrow(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse)
{
	UNREFERENCED_PARAMETER(eventData);

	ResetVelocity();

	AddForceToVelocity(throwImpulse);

	m_deltaTime = 5.0f;

	m_isHold = false;
	m_isThrow = true;
}


void Enemy::OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);

	// 矩形にキャスト
	const AABB* pHitBox = dynamic_cast<const AABB*>(pHitCollider);


	// 押し出しの計算
	SimpleMath::Vector3 overlap = CalcOverlap(*pHitBox, *m_collider);


	SimpleMath::Vector3 overlapDir = overlap;
	overlapDir.Normalize();


	// 現在の押し出しの単位ベクトル
	SimpleMath::Vector3 totalOverlapNormal = m_overlapTotal;
	totalOverlapNormal.Normalize();

	// 押し出し方向ベクトル（累積方向 + 今回の方向）
	SimpleMath::Vector3 combinedDirection = totalOverlapNormal + overlapDir;

	if (combinedDirection.LengthSquared() < 1e-6f)
		return;

	combinedDirection.Normalize();

	// それぞれの押し出しを、合成方向に投影して必要量を求める
	float push1 = std::max(0.0f, overlap.Dot(combinedDirection));
	float push2 = std::max(0.0f, m_overlapTotal.Dot(combinedDirection));

	// より大きな押し出し量を採用
	float projectedDepth = std::max(push1, push2);

	// 合成押し出しを更新
	m_overlapTotal += combinedDirection * projectedDepth;

}
