/*****************************************************************//**
 * @file    Wire.h
 * @brief   ワイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/07/02
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Wire.h"

#include "Library/MyLib/DirectXMathUtils/DirectXMathUtils.h"

// 共有リソース
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"

// オブザーバー関連
#include "Game/Common/Event/WireSystemObserver/WireSystemSubject.h"
#include "Game/Common/Event/WireSystemObserver/IWireEventObserver.h"

#include "Game/Common/Collision/Collision.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"

#include "Game/GameObjects/Wire/IWireHolder/IWireHolder.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraintFactory.h"


using namespace DirectX;

# define DEBUG

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Wire::Wire()
	: m_pCollisionManager{ nullptr }
	, m_isActive{ false }
	, m_wireSystemSubject{ nullptr }
	, m_owner{}
	, m_length {}
{

}



/**
 * @brief デストラクタ
 */
Wire::~Wire()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] pCommonResources　共通リソース
 * @param[in] pCollisionManager	衝突管理
 * @param[in] simulationParam　	シミュレーションのパラメータ
 * @param[in] pOwnerGameObject	所有者のゲームオブジェクト
 * @param[in] pHolderInterface	保持者のインターフェース
 * @param[out] wireSystemSubject ワイヤーシステムの観察者
 */
void Wire::Initialize(
	CommonResources*	pCommonResources,
	CollisionManager*	pCollisionManager,
	const XPBDSimulator::Parameter& simulationParam,
	const float& length,
	const GameObject*	pOwnerGameObject,
	const MovableObject* pMovableObject,
	IWireHolder*		pHolderInterface,
	WireSystemSubject*	wireSystemSubject)
{
	// リソース管理の設定
	SetCommonResources(pCommonResources);

	m_owner.pGameObject		= pOwnerGameObject;
	m_owner.pMovableObject	= pMovableObject;
	m_owner.pHolderInterface = pHolderInterface;

	// ワイヤーシステムの観察対象を設定
	m_wireSystemSubject = wireSystemSubject;

	// コライダの作成
	m_collider = std::make_unique<Segment>();

	// 衝突管理の設定
	m_pCollisionManager = pCollisionManager;

	// ロープオブジェクトの作成
	m_ropeObject = std::make_unique<RopeObject>();
	m_ropeObject->Initialize(pCommonResources);
	// シミュレータ作成
	m_simulator = std::make_unique<XPBDSimulator>();

	// 制約の追加
	std::vector<std::unique_ptr<ConstraintFactory>> constraintFactories;
	constraintFactories.emplace_back(std::make_unique<CollisionConstraintFactory>(m_pCollisionManager, simulationParam));
	m_simulator->SetConstraint(&constraintFactories);

	m_length = length;

	m_simulationParam = simulationParam;

	m_isActive = false;

	m_simulator->SetCollisionManager(pCollisionManager);

}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void Wire::Update(float elapsedTime)
{

	if (m_isExtention)
	{

		auto particle = m_ropeObject->GetParticles()->back();
		SimpleMath::Vector3 pos = particle->GetPosition() + m_wireVelocity * elapsedTime;

		float lengthSqr = SimpleMath::Vector3::DistanceSquared(m_owner.pGameObject->GetPosition(), pos);

		if (lengthSqr >= m_length * m_length)
		{
			Reset();
			m_isExtention = false;
			return;
		}


		particle->SetPosition(pos);
		m_ropeObject->GetParticles()->front()->SetPosition(m_owner.pGameObject->GetPosition());
		m_collider->Set(m_owner.pGameObject->GetPosition(), pos,true);


	}

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Wire::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	auto states = GetCommonResources()->GetCommonStates();
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();


	m_ropeObject->Draw(view, proj);

	SimpleMath::Matrix world;
# ifdef DEBUG
	static bool debug = false;
	auto kb = GetCommonResources()->GetKeyboardTracker();

	if (kb->IsKeyPressed( Keyboard::U)) debug = !debug;

	if (debug)
	for (const auto& obj : m_particleObjects)
	{
		Model ballModel = GetCommonResources()->GetResourceManager()->CreateModel("Ball.sdkmesh");

		world = SimpleMath::Matrix::CreateScale(0.05f);
		world *= SimpleMath::Matrix::CreateTranslation(obj->GetPosition());
		ballModel.Draw(context, *states, world, view, proj);
	}

# else
	if (m_particleObjects.size() > 0)
	{
		// モデルの描画
		Model ballModel = GetCommonResources()->GetResourceManager()->CreateModel("Ball.sdkmesh");

		world = SimpleMath::Matrix::Identity;
		world = SimpleMath::Matrix::CreateScale(0.5f);
		world *= SimpleMath::Matrix::CreateTranslation(m_particleObjects[0]->GetPosition());
		ballModel.Draw(context, *states, world, view, proj);
	}

# endif

	if (m_particleObjects.size() > 0)
		m_ropeObject->Draw(view, proj);

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Wire::Finalize()
{

}

/**
 * @brief リセット
 * 
 */
void Wire::Reset()
{
	m_ropeObject->ResetParticle();
	m_simulator->Reset();

	m_particleObjects.clear();

	m_isExtention = false;
}

/**
 * @brief シミュレータの適用
 * 
 * @param[in] elapsedTime　経過時間
 */
void Wire::ApplyWireSimulator(const float& elapsedTime)
{
	if (m_simulator.get() != nullptr)
		m_simulator->Update(elapsedTime);
}

/**
 * @brief ワイヤーの発射
 * 
 * @param[in] origin	   発射地点
 * @param[in] wireVelocity ワイヤー速度
 */
void Wire::ShootWire(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& wireVelocity)
{
	using namespace SimpleMath;

	m_extentionRay.origin = origin;
	m_extentionRay.direction = wireVelocity;
	m_extentionRay.direction.Normalize();

	m_isActive = true;
	m_isExtention = true;
	m_wireVelocity = wireVelocity;

	m_collider->Set(origin, origin,true);

	m_pCollisionManager->AddCollisionObjectData(this, m_collider.get());

	m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
	m_particleObjects.back()->SetPosition(origin);
	m_ropeObject->AddParticle(m_particleObjects.back().get());

	m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
	m_particleObjects.back()->SetPosition(origin);
	m_ropeObject->AddParticle(m_particleObjects.back().get());


}

/**
 * @brief ワイヤの作成
 *
 * @param[in] origin    ワイヤーがくっつく点
 * @param[in] direction
 * @param[in] particleNum
 * @param[in] length
 * @param[in] param
 */
bool Wire::CreateRope(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& direction, const int& particleNum, const float& length, const XPBDSimulator::Parameter& param)
{
	using namespace SimpleMath;

	// 長さが指定値未満だったら生成しない

	if (particleNum <= 0) { return true; }

	int particleNumTmp = particleNum /2 ;

	// 配列をリセット
	m_particleObjects.clear();
	for (int i = 0; i < particleNumTmp; i++)
	{
		float distance = (length / particleNumTmp) * i;


		// 原点からの場所
		Vector3 originToPos = direction * distance;

		// 位置
		Vector3 pos{};
		pos = origin + distance * direction;

		m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
		m_particleObjects.back()->SetPosition(pos);

		float mass = (5.0f / particleNumTmp) * (1 + particleNumTmp - i);
		m_particleObjects.back()->SetMass(5.0f + mass);
		m_particleObjects.back()->SetVelocity(m_owner.pMovableObject->GetVelocity() * (static_cast<float>(i) + 1.0f )/ static_cast<float>(particleNumTmp));



	}


	if (m_particleObjects.size() <= 3)
	{
		m_particleObjects.clear();
		return false;
	}

	m_ropeObject->Initialize(GetCommonResources());

	for (auto& obj : m_particleObjects)
	{
		m_ropeObject->AddParticle(obj.get());

	}

	

	//m_collider->Set(origin, origin,true);

	//m_pCollisionManager->AddCollisionObjectData(this, m_collider.get());

	// シュミレーションの初期化
	m_simulator->Initialize(param, m_ropeObject.get());
	return true;
}

/**
 * @brief ロープの生成
 *
 * @param[in] origin		ロープの発射地点
 * @param[in] targetPoint	ロープの目標地点
 * @param[in] param
 * @param[in] ratio			間隔の比率　1.0だとただの直線になる
 */
bool Wire::CreateRope(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& targetPoint, const XPBDSimulator::Parameter& param, float particleDistance)
{
	using namespace SimpleMath;

	// ターゲットまでのベクトル
	Vector3 targetToOriginDir = origin - targetPoint;

	// 長さを求める
	float length = targetToOriginDir.Length();

	int particleNum = static_cast<int>(length / particleDistance);

	targetToOriginDir.Normalize();

	return CreateRope(targetPoint, targetToOriginDir, particleNum, length, param);


}

/**
 * @brief 衝突処理
 * 
 * @param[in] pHitObject
 * @param[in] pHitCollider
 */
void Wire::OnCollision(GameObject* pHitObject, ICollider* pHitCollider)
{

	//// **** ワイヤーが掴んだオブジェクトによる状態の変更 ****
	//if (it_minDistance->hitObj->GetTag() == GameObjectTag::WALL)
	//{
	//	m_stateMachine->ChangeState<WireActionPlayerState>();
	//	// **** ワイヤーの作成 *****
	//	CreateRope(GetPosition(), intersectionPos, param, 0.3f);
	//}
	//
	if (m_isExtention)
	{
		SimpleMath::Vector3 intersectionPos;

		//矩形の場合
		if (pHitCollider->GetColliderType() == ColliderType::BOX2D)
		{
			if (const Box2D* pBox = dynamic_cast<const Box2D*>(pHitCollider))
			{
				if (GetIntersectionPoint(&intersectionPos, *pBox, *m_collider) == false) return;

			}

		}
		// 球の場合
		else if (pHitCollider->GetColliderType() == ColliderType::Sphere)
		{
			if (const Sphere* pSphere = dynamic_cast<const Sphere*>(pHitCollider))
			{
				if (GetIntersectionPoint(&intersectionPos, *pSphere, *m_collider) == false) return;

			}
		}
		else if (pHitCollider->GetColliderType() == ColliderType::AABB)
		{
			if (const AABB* pAABB = dynamic_cast<const AABB*>(pHitCollider))
			{
				if (GetIntersectionPoint(&intersectionPos, *pAABB, *m_collider) == false) return;

			}
		}

	

		if (pHitObject->GetTag() == GameObjectTag::WALL ||
			pHitObject->GetTag() == GameObjectTag::BUILDING ||
			pHitObject->GetTag() == GameObjectTag::ESCAPE_HELICOPTER)
		{


			// **** ワイヤーの作成 *****
			if (CreateRope(m_owner.pGameObject->GetPosition(), intersectionPos, m_simulationParam, 0.9f))
			{

				m_owner.pHolderInterface->OnCollisionWire(pHitObject);
				m_isExtention = false;
			}
		}



		else if (auto observer = pHitObject->CastTo<IWireEventObserver>())
		{
			if (m_wireSystemSubject)
			m_wireSystemSubject->AddObserver(observer);
			
			m_owner.pHolderInterface->OnWireGrabbed(pHitObject);

			// **** ワイヤーの作成 *****
			//CreateRope(m_owner.pGameObject->GetPosition(), intersectionPos, m_simulationParam, 0.3f);


			m_pCollisionManager->RemoveCollisionObjectData(this, m_collider.get());
			m_isExtention = false;

		}
	}
}

/**
 * @brief ワイヤーの終端座標の取得
 * 
 * @return 終端座標
 */
DirectX::SimpleMath::Vector3 Wire::GetEndPosition() const
{
	return m_ropeObject->GetParticles()->back()->GetPosition();
}

DirectX::SimpleMath::Vector3 Wire::GetStartVelocity() const
{
	return m_ropeObject->GetParticles()->back()->GetVelocity();
}

ParticleObject* Wire::GetBackPivot() const
{
	return m_ropeObject->GetParticles()->back();
}

ParticleObject* Wire::GetFrontPivot() const
{
	return m_ropeObject->GetParticles()->front();
}

