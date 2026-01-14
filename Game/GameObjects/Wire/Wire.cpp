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

#include "Game/Common/Camera/Camera.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/DistanceConstraint/DistanceConstraintFactory.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraintFactory.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/SteeringConstraint/SteeringConstraintFactory.h"


using namespace DirectX;

# define DEBUG
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
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
	, m_wireSpeed{ 0.f }
	, m_isExtention{ false }
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
	const CommonResources*	pCommonResources,
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

	// **** シミュレータ作成 ****
	m_simulator = std::make_unique<XPBDSimulator>();
	// **** 制約の追加 ****
	// 操舵制約
	m_simulator->AddConstraint(std::make_unique<SteeringConstraintFactory> (GetCommonResources()));	
	// 距離制約
	m_simulator->AddConstraint(std::make_unique<DistanceConstraintFactory> ());						
	// 衝突制約
	m_simulator->AddConstraint(std::make_unique<CollisionConstraintFactory>(m_pCollisionManager));	


	m_length = length;

	m_simulationParam = simulationParam;

	m_isActive = false;

	m_simulator->SetCollisionManager(pCollisionManager);

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void Wire::Update(float deltaTime)
{
	using namespace DirectX::SimpleMath;

	if (m_isExtention)
	{

		//// 最後尾（飛んでいる）のパーティクルを取得する
		auto particle = m_ropeObject->GetParticles()->back();

		// lerpTimeの更新
		m_totalLerpTime += m_lerpSpeed * deltaTime;


		m_totalLerpTime = std::min(m_totalLerpTime, 1.0f);
	
		auto pos = Vector3::Lerp(m_owner.pGameObject->GetTransform()->GetPosition(), m_wireTargetPosition, m_totalLerpTime);

		particle->SetPosition(pos);
		m_ropeObject->GetParticles()->front()->SetPosition(m_owner.pGameObject->GetTransform()->GetPosition());
		m_collider->Set(m_owner.pGameObject->GetTransform()->GetPosition(), pos,true);

		if (m_totalLerpTime >= 1.0f)
		{


			// **** ワイヤーの作成 *****
			if (CreateRope(m_owner.pGameObject->GetTransform()->GetPosition(), m_wireTargetPosition, m_simulationParam, 0.8f))
			{

				m_owner.pHolderInterface->OnCollisionWire(nullptr);
				m_isExtention = false;
			}
			m_isExtention = false;
//			Reset();

		}

	}

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Wire::Draw(const Camera& camera)
{
	if (m_isActive == false) { return; }
	auto states = GetCommonResources()->GetCommonStates();
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();

	m_ropeObject->Draw(camera);

	SimpleMath::Matrix world;
# ifdef DEBUG
	static bool debug = false;
	auto kb = GetCommonResources()->GetKeyboardTracker();

	if (kb->IsKeyPressed( Keyboard::U)) debug = !debug;

	if (debug)
	{
		for (const auto& obj : m_particleObjects)
		{
			Model ballModel = GetCommonResources()->GetResourceManager()->CreateModel("Ball.sdkmesh");

			world = SimpleMath::Matrix::CreateScale(0.05f);
			world *= SimpleMath::Matrix::CreateTranslation(obj->GetPosition());
			ballModel.Draw(context, *states, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		}

		GetCommonResources()->GetDebugFont()->AddString(10, 50, Colors::Red, L"particle velocity : (%f, %f, %f) ", m_particleObjects.back()->GetVelocity().x, m_particleObjects.back()->GetVelocity().y, m_particleObjects.back()->GetVelocity().z);
		GetCommonResources()->GetDebugFont()->AddString(10, 70, Colors::Red, L"particle position : (%f, %f, %f) ", m_particleObjects.back()->GetPosition().x, m_particleObjects.back()->GetPosition().y, m_particleObjects.back()->GetPosition().z);
	}


# else
	if (m_particleObjects.size() > 0)
	{
		// モデルの描画
		Model ballModel = GetCommonResources()->GetResourceManager()->CreateModel("Ball.sdkmesh");

		world = SimpleMath::Matrix::Identity;
		world = SimpleMath::Matrix::CreateScale(0.5f);
		world *= SimpleMath::Matrix::CreateTranslation(m_particleObjects[0]->GetPosition());
		ballModel.Draw(context, *states, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}

# endif

	if (m_particleObjects.size() > 0)
		m_ropeObject->Draw(camera);

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

	m_isActive = false;

	m_lerpSpeed = 0.0f;
	m_totalLerpTime = 0.0f;

}

/**
 * @brief シミュレータの適用
 * 
 * @param[in] deltaTime　経過時間
 */
void Wire::ApplyWireSimulator(const float& deltaTime)
{
	if (m_simulator.get() != nullptr)
		m_simulator->Update(deltaTime);
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

	m_pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get()));

	m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
	m_particleObjects.back()->SetPosition(origin);
	m_ropeObject->AddParticle(m_particleObjects.back().get());

	m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
	m_particleObjects.back()->SetPosition(origin);
	m_ropeObject->AddParticle(m_particleObjects.back().get());


}

void Wire::ShootWireToTarget(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& targetPosition, const float& speed)
{
	using namespace SimpleMath;

	Reset();

	m_extentionRay.origin = origin;
	Vector3 direction = targetPosition - origin;
	direction.Normalize();
	m_extentionRay.direction = direction;
	m_extentionRay.direction.Normalize();

	m_wireTargetPosition = targetPosition;
	m_wireSpeed = speed;

	m_isActive = true;
	m_isExtention = true;
	m_wireVelocity = direction;

	m_collider->Set(origin, origin, true);

	m_pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get()));

	m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
	m_particleObjects.back()->SetPosition(origin);
	m_ropeObject->AddParticle(m_particleObjects.back().get());

	m_particleObjects.emplace_back(std::make_unique<ParticleObject>());
	m_particleObjects.back()->SetPosition(origin);
	m_ropeObject->AddParticle(m_particleObjects.back().get());

	Vector3 endPosition = m_wireTargetPosition;
	Vector3 startPosition = m_owner.pGameObject->GetTransform()->GetPosition();

	m_lerpSpeed = m_wireSpeed / SimpleMath::Vector3::Distance(startPosition, endPosition);
	m_totalLerpTime = 0.0f;
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
 * @param[in] info 衝突情報
 */
void Wire::OnCollision(const CollisionInfo& info)
{
	UNREFERENCED_PARAMETER(info);
	
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

/**
 * @brief 原点座標の設定
 * 
 * @param[in] origin
 */
void Wire::SetOriginPosition(const DirectX::SimpleMath::Vector3& origin)
{
	m_extentionRay.origin = origin;
}

