/*****************************************************************//**
 * @file    SpawnManager.h
 * @brief   出現管理クラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SpawnManager.h"

#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"

#include "Game/GameObjects/Enemy/Enemy.h"

#include "Game/Common/GameObjectManager/EnemyManager/EnemyManager.h"
#include "Game/Common/Factory/EnemyFactory/EnemyFactory.h"
#include "Game/Common/GameObjectRegistry/GameObjectRegistry.h"
#include "Game/GameObjects/Helicopter/EscapeHelicopter/EscapeHelicopter.h"
#include <random>

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SpawnManager::SpawnManager()
	: m_pEnemyManager		{ nullptr }
	, m_pCommonResources	{ nullptr }
	, m_pCollisionManager	{ nullptr }
	, m_stoleTreasure		{ false }
	, m_pEscapeHelicopters	{}
{

}



/**
 * @brief デストラクタ
 */
SpawnManager::~SpawnManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SpawnManager::Initialize(
	EnemyManager* pEnemyManager,
	std::vector<std::unique_ptr<EscapeHelicopter>>* pEscapeHelicopters,const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	m_pEnemyManager = pEnemyManager;
	m_pEscapeHelicopters = pEscapeHelicopters;

	m_pCommonResources = pCommonResources;
	m_pCollisionManager = pCollisionManager;

	// オブザーバーに登録
	GameFlowMessenger::GetInstance()->RegistryObserver(this);

	// イベントスタックの初期化処理
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool SpawnManager::UpdateTask(float deltaTime)
{

	if (m_stoleTreasure)
	{
		m_enemySpawnCounter.UpperTime(deltaTime);

		if (m_enemySpawnCounter.GetElapsedTime() >= 10.0f)
		{
			SpawnEnemy();
			m_enemySpawnCounter.Reset();
		}
	}

	for (auto& eventFunc : m_eventStack)
	{
		eventFunc();
	}
	m_eventStack.clear();

	return true;
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SpawnManager::Finalize()
{

}

/**
 * @brief イベントメッセージを受け取る
 * 
 * @param[in] eventID イベントID
 */
void SpawnManager::OnGameFlowEvent(GameFlowEventID eventID)
{
	switch (eventID)
	{
	case GameFlowEventID::STOLE_TREASURE:
		// イベントの登録
		m_eventStack.emplace_back([this]() {
			OnStealingTreasures();
			});		break;
	default:
		break;
	}
}

/**
 * @brief 宝が盗まれた時の処理
 * 
 */
void SpawnManager::OnStealingTreasures()
{
	m_stoleTreasure = true;
	using namespace SimpleMath;

	// プレイヤーの取得
	auto pPlayer = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER);
	if (pPlayer == nullptr) { return; }

	//　****　敵の生成 ****
	Vector3 center = pPlayer->GetTransform()->GetPosition();

	std::unique_ptr<IEnemyFactory> factory = std::make_unique<EnemyFactory::FlyingChaserEnemy>();

	const int NUM = 10;
	const float RADIUS = 20.0f;

	for (int i = 0; i < NUM; i++)
	{
		float degree = (360.0f / NUM) * i;

		auto enemy = factory->Create();
		
		enemy->Initialize(m_pCommonResources, m_pCollisionManager);
		Vector3 pos;

		pos.x = center.x + std::cosf(XMConvertToRadians(degree)) * RADIUS;
		pos.z = center.z +std::sinf(XMConvertToRadians(degree)) * RADIUS;
		pos.y = center.y;
		
		enemy->GetTransform()->SetPosition(pos);

		m_pEnemyManager->AddEnemy(enemy.get());

		m_enemyPool.push_back(std::move(enemy));
	}

	// ハードウェア乱数源からシードを生成
	static std::random_device seed_gen;

	// シードを使って乱数エンジンを初期化
	std::mt19937 engine(seed_gen());
	
	std::vector<Vector3> helicopterPositions =
	{
		Vector3(-90.0f, 80.6f, 70.2f),
		Vector3(130.0f, 110.6f, 120.2f),
		Vector3(-100.0f, 60.6f, -130.2f),
		Vector3(160.0f, 60.6f, -100.2f),
	};
	// 基準点から遠い順にソート
	std::sort(helicopterPositions.begin(), helicopterPositions.end(),
		[&](const Vector3& a, const Vector3& b)
		{
			// 基準点からの距離を比較
			float distanceA = DirectX::SimpleMath::Vector3::DistanceSquared(a, center);
			float distanceB = DirectX::SimpleMath::Vector3::DistanceSquared(b, center);
			return distanceA > distanceB; // 遠い順なので > を使用
		});

	std::vector<float> weights = { 0.5f, 0.25f, 0.15f, 0.0f}; // 例: 0番目が50%の確率で出る
	std::discrete_distribution<> dist(weights.begin(), weights.end());

	auto position = helicopterPositions.front();//helicopterPositions[selectedIndex];
	

	// 脱出用ヘリコプターの生成
	m_pEscapeHelicopters->emplace_back(std::make_unique<EscapeHelicopter>());
	m_pEscapeHelicopters->back()->Initialize(m_pCommonResources, m_pCollisionManager);
	m_pEscapeHelicopters->back()->GetTransform()->SetPosition(position);

	// ヘリコプター出現を通知
	GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::SPAWN_HELICOPTER);

	// タスクの追加
	GetTaskManager()->AddTask(m_pEscapeHelicopters->back().get());
}

void SpawnManager::SpawnEnemy()
{
	using namespace SimpleMath;

	//　****　敵の生成 ****
	Vector3 treasurePos = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER)->GetTransform()->GetPosition();
	std::uniform_int_distribution<> dist(0, 180);
	std::unique_ptr<IEnemyFactory> factory = std::make_unique<EnemyFactory::FlyingChaserEnemy>();

	// ハードウェア乱数源からシードを生成
	static std::random_device seed_gen;
	// シードを使って乱数エンジンを初期化
	std::mt19937 engine(seed_gen());

	const int NUM = 3;
	const float RADIUS = 20.0f;

	int startDegree = dist(engine);

	for (int i = 0; i < NUM; i++)
	{
		float degree = (360.0f / NUM) * i + startDegree;

		auto enemy = factory->Create();

		enemy->Initialize(m_pCommonResources, m_pCollisionManager);
		Vector3 pos;

		pos.x = treasurePos.x + std::cosf(XMConvertToRadians(degree)) * RADIUS;
		pos.z = treasurePos.z + std::sinf(XMConvertToRadians(degree)) * RADIUS;
		pos.y = treasurePos.y;

		enemy->GetTransform()->SetPosition(pos);

		m_pEnemyManager->AddEnemy(enemy.get());

		m_enemyPool.push_back(std::move(enemy));
	}

}


