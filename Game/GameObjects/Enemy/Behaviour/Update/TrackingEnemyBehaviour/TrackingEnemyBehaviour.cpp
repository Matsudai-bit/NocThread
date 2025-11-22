/*****************************************************************//**
 * @file    TrackingEnemyBehaviour.h
 * @brief   敵の追跡挙動に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TrackingEnemyBehaviour.h"

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
TrackingEnemyBehaviour::TrackingEnemyBehaviour()
{
	m_playerTargetTimeCounter.Reset();

	// ハードウェア乱数源からシードを生成
	std::random_device seed_gen;

	// シードを使って乱数エンジンを初期化
	std::mt19937 engine(seed_gen());

	// 0.0fから2.0fの範囲で一様分布を定義
	std::uniform_real_distribution<float> dist(-0.4f, 0.6f);

	// 乱数を生成し、返す

	m_playerTargetTimeCounter.UpperTime(dist(engine));
}



/**
 * @brief デストラクタ
 */
TrackingEnemyBehaviour::~TrackingEnemyBehaviour()
{

}

/**
 * @brief 更新処理
 * 
 * @param[in] pEnemy		敵（自身）
 * @param[in] deltaTime	経過時間
 * @param[in] pPlayer		プレイヤー
 */
void TrackingEnemyBehaviour::Update(Enemy* pEnemy, float deltaTime, const CommonResources* pCommonResources)
{
	UNREFERENCED_PARAMETER(pCommonResources);
	UNREFERENCED_PARAMETER(deltaTime);
	using namespace SimpleMath;

	m_playerTargetTimeCounter.UpperTime(deltaTime);

	if (m_playerTargetTimeCounter.GetElapsedTime() >= 1.0f)
	{
		const GameObject* pPlayerObject = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER);
		if (pPlayerObject == nullptr) {
			return;
		}

		Vector3 playerPosition = pPlayerObject->GetTransform()->GetPosition();

		m_targetDirection = playerPosition - pEnemy->GetTransform()->GetPosition();

		// Y軸方向の情報を消す
		m_targetDirection.y = 0.0f;

		m_targetDirection.Normalize();
		m_playerTargetTimeCounter.Reset();

	}

	
	const float ACCELERATION = 130.0f;				// 加速力（加速の強さ）
	const float DECELERATION = 0.0f;				// 減速力（ブレーキの強さ）
	const float MAX_MOVE_SPEED = 21.0f;					// 最大速度

		

	Vector3 velocity = MovementHelper::ClampedMovement(pEnemy->GetVelocity() * Vector3(1.0f, 0.0f, 1.0f), m_targetDirection, deltaTime, ACCELERATION, DECELERATION, MAX_MOVE_SPEED); ;

	

	// 速度の適用
	pEnemy->AddForceToVelocity(velocity);


	
}


