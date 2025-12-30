/*****************************************************************//**
 * @file    FlyingChaserEnemyBehaviour.h
 * @brief   敵の空中追跡挙動に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "FlyingChaserEnemyBehaviour.h"


#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Player/Player.h"
#include "Game/Common/GameObjectRegistry/GameObjectRegistry.h"

#include "Game/Common/Helper/MovementHelper/MovementHelper.h"
#include <random>

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
FlyingChaserEnemyBehaviour::FlyingChaserEnemyBehaviour()
	: m_steeringBehavior{ 18.0f, 0.000001f}
{
	m_playerTargetTimeCounter.Reset();

	// ハードウェア乱数源からシードを生成
	static std::random_device seed_gen;

	// シードを使って乱数エンジンを初期化
	std::mt19937 engine(seed_gen());

	// 0.0fから2.0fの範囲で一様分布を定義
	std::uniform_real_distribution<float> dist(-0.4f, 0.6f);

	// 乱数を生成し、返す

	m_playerTargetTimeCounter.UpperTime(dist(engine) );
}



/**
 * @brief デストラクタ
 */
FlyingChaserEnemyBehaviour::~FlyingChaserEnemyBehaviour()
{

}

/**
 * @brief 更新処理
 * 
 * @param[in] pEnemy		敵（自身）
 * @param[in] deltaTime	経過時間
 * @param[in] pPlayer		プレイヤー
 */
void FlyingChaserEnemyBehaviour::Update(Enemy* pEnemy, float deltaTime, const CommonResources* pCommonResources)
{
	UNREFERENCED_PARAMETER(pCommonResources);
	UNREFERENCED_PARAMETER(deltaTime);
	using namespace SimpleMath;



	m_playerTargetTimeCounter.UpperTime(deltaTime);

	//if (m_playerTargetTimeCounter.GetElapsedTime() >= 1.0f)
	//{
		const GameObject* pPlayerObject = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER);
		if (pPlayerObject == nullptr) {
			return;
		}

		Vector3 playerPosition = pPlayerObject->GetTransform()->GetPosition();

		m_steeringBehavior.SetTargetPosition(playerPosition);
		m_targetDirection = playerPosition - pEnemy->GetTransform()->GetPosition();

		// Y軸方向の情報を消す
		m_targetDirection.Normalize();
		m_playerTargetTimeCounter.Reset();

	/*}*/



	// 水平方向
	SimpleMath::Vector3 horizonalDirection	= m_targetDirection * Vector3(1.0f, 0.0f, 1.0f);
	// 飛ぶ方向
	SimpleMath::Vector3 flyingDirection		= m_targetDirection * Vector3(0.0f, 1.0f, 0.0f);

	flyingDirection.Normalize();
	horizonalDirection.Normalize();

	
	const float ACCELERATION = 130.0f;				// 加速力（加速の強さ）
	const float DECELERATION = 0.0f;				// 減速力（ブレーキの強さ）
	const float MAX_MOVE_SPEED = 18.0f;					// 最大速度

	const float MAX_FLYING_SPEED = 15.0f;					// 最大速度

		
	// 水平方向へ移動する速度を算出
	Vector3 velocity = MovementHelper::ClampedMovement(pEnemy->GetVelocity() * Vector3(1.0f, 0.0f, 1.0f), horizonalDirection, deltaTime, ACCELERATION, DECELERATION, MAX_MOVE_SPEED); ;

	pEnemy->AddForceToVelocity(velocity);


	// 飛ぶ向きに対する速度を算出
	Vector3 flyingVelocity = MovementHelper::ClampedMovement(pEnemy->GetVelocity() * Vector3(0.0f, 1.0f, 0.0f), flyingDirection, deltaTime, ACCELERATION, DECELERATION, MAX_FLYING_SPEED);

	// 下向きの動きの場合少し遅くする
	if (flyingDirection.y < 0.0f)
	{
		flyingDirection = flyingDirection / 0.8f;
	}

	// 速度の適用
	pEnemy->AddForceToVelocity(flyingVelocity);


	DirectX::SimpleMath::Vector3 steeringForce = m_steeringBehavior.Calculate(pEnemy->GetTransform()->GetPosition(), pEnemy->GetVelocity());
	pEnemy->AddForceToVelocity(steeringForce);
}


