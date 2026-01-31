/*****************************************************************//**
 * @file    SpawnManager.h
 * @brief   出現管理クラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <random>
#include <fstream>
#include "SpawnManager.h"

// ライブラリ関連
#include "Library/MyLib/NlohmannUtils/NlohmannUtils.h"

// ファクトリー関連
#include "Game/Common/Factory/PlayerFactory/PlayerFactory.h"
#include "Game/Common/Factory/BuildingFactory/BuildingFactory.h"
#include "Game/Common/Factory/EnemyFactory/EnemyFactory.h"

// フレームワーク関連
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Framework/GameObjectManager/EnemyManager/EnemyManager.h"
#include "Game/Common/Framework/GameObjectRegistry/GameObjectRegistry.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Helicopter/EscapeHelicopter/EscapeHelicopter.h"
#include "Game/GameObjects/Player/Player.h"


#include "Game/Common/Framework/GameObjectManager/BuildingManager/BuildingManager.h"
#include "Game/Common/Framework/GameObjectManager/PlayerManager/PlayerManager.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SpawnManager::SpawnManager()
	: m_pEnemyManager		{ nullptr }
	, m_pPlayerManager		{ nullptr }
	, m_pBuildingManager	{ nullptr }
	, m_pCommonResources	{ nullptr }
	, m_pCollisionManager	{ nullptr }
	, m_pPlayerCamera		{ nullptr }
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
void SpawnManager::Initialize(const CommonResources* pCommonResources,CollisionManager* pCollisionManager)
{
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

void from_json(const nlohmann::json& j, SpawnManager::StageLayoutData& data)
{
	j.at("BuildingData").get_to(data.buildingJsonName);
	j.at("PlayerData").get_to(data.playerJsonName);
}
void from_json(const nlohmann::json& j, SpawnManager::PlayerData& data)
{
	j.at("PlaceTileNumber").get_to(data.tileNumber);
}
/**
 * @brief ゲームの初期配置をする
 */
void SpawnManager::SetupInitialLayout()
{

	// ステージのレイアウトデータまでのパス
	const std::string stageLayoutDataPath = STAGE_DATA_FOLDER_PATH + "/" + "stageLayoutData.json";

	// ステージデータのjsonを読み込んでStageLayoutData型に変換する
	StageLayoutData stageLayoutData;
	if (!MyLib::NlohmannUtils::TryLoadAndConvertJson<StageLayoutData>(stageLayoutDataPath, &stageLayoutData)) { return; }

	// プレイヤーデータまでのパス
	const std::string playerDataPath = STAGE_DATA_FOLDER_PATH + "/" + stageLayoutData.playerJsonName;
	PlayerData playerData;
	// プレイヤーデータデータのjsonを読み込んでPlayerData型に変換する
	if (!MyLib::NlohmannUtils::TryLoadAndConvertJson<PlayerData>(playerDataPath, &playerData)) { return; }

	// **** 建物の生成 ****
	{
		TownFactory townFactory;
		std::string buildingJsonPath = STAGE_DATA_FOLDER_PATH + "/" + stageLayoutData.buildingJsonName;
		auto buildings = townFactory.Create(TownFactoryDesk{ m_pCollisionManager, *m_pCommonResources, buildingJsonPath });
		m_pBuildingManager->SetBuildings(std::move(buildings));
	}

	
	// **** プレイヤーの生成 ****
	{
		PlayerFactory::StagePlayer playerFactory;
		auto player = playerFactory.Create(PlayerFactory::PlayerDesk{
			*m_pCommonResources,
			m_pCollisionManager,
			*m_pBuildingManager,
			playerData.tileNumber,
			m_pPlayerCamera });

		m_pPlayerManager->SetPlayer(std::move(player));
	}
}



/**
 * @brief ゲームオブジェクト管理の設定
 * 
 * @param[in] pPlayerManager	プレイヤー管理
 * @param[in] pBuildingManager	建物管理
 * @param[in] pEnemyManager		敵管理
 * @param[in] pEscapeHelicoptersヘリコプター
 * @param[in] pPlayerCamera		プレイヤーカメラ
 */
void SpawnManager::SetManagers(
	PlayerManager* pPlayerManager,
	BuildingManager* pBuildingManager,
	EnemyManager* pEnemyManager,
	std::vector<std::unique_ptr<EscapeHelicopter>>* pEscapeHelicopters,
	PlayerCamera* pPlayerCamera)
{
	m_pPlayerManager = pPlayerManager;
	m_pBuildingManager = pBuildingManager;
	m_pEnemyManager = pEnemyManager;
	m_pEscapeHelicopters = pEscapeHelicopters;
	m_pPlayerCamera = pPlayerCamera;
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
	case GameFlowEventID::GAME_START:
		// イベントの登録
		m_eventStack.emplace_back([this]() {
			OnGameStartSpawn();
			});		break;
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

	auto factory = std::make_unique<EnemyFactory::FlyingChaserEnemy>();

	const int NUM = 3;
	const float RADIUS = 20.0f;

	for (int i = 0; i < NUM; i++)
	{
		float degree = (360.0f / NUM) * i;

		auto enemy = factory->Create(DefaultSpawnDesc());
		
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

void SpawnManager::OnGameStartSpawn()
{
	// 左下に敵の生成
	auto factory = std::make_unique<EnemyFactory::FlyingChaserEnemy>();

	auto enemy = factory->Create(DefaultSpawnDesc());
	enemy->Initialize(m_pCommonResources, m_pCollisionManager);
	enemy->GetTransform()->SetPosition(DirectX::SimpleMath::Vector3(0.0f, 20.0f, 200.0f));
	m_pEnemyManager->AddEnemy(enemy.get());
	m_enemyPool.push_back(std::move(enemy));


}

void SpawnManager::SpawnEnemy()
{
	using namespace SimpleMath;

	//　****　敵の生成 ****
	Vector3 treasurePos = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER)->GetTransform()->GetPosition();
	std::uniform_int_distribution<> dist(0, 180);
	auto factory = std::make_unique<EnemyFactory::FlyingChaserEnemy>();

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

		auto enemy = factory->Create(DefaultSpawnDesc());

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


