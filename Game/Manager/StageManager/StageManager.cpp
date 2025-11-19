/*****************************************************************//**
 * @file    StageManager.h
 * @brief   ステージ管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/11/17
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "StageManager.h"

// 定数ヘッダーをインクルード

#include "Game/Common/Camera/MainCamera/MainCamera.h"
#include "Game/Scene/TitleScene/TitleScene.h"
#include "Game/Scene/ResultScene/ResultScene.h"
#include "Game/Common/DeviceResources.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/Common/Collision/Collision.h"
#include "Game/Common/SoundManager/SoundManager.h"
#include "Game/Common/SoundManager/SoundPaths.h"
#include "Game/Scene/Loading/LoadingScreen.h"

#include "Game/Common/Shadow/CircularShadow/CircularShadow.h"

// ゲームオブジェクト
#include "Game/GameObjects/Floor/Floor.h"
#include "Game/GameObjects/Wall/Wall.h"		// 壁
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/StageObject/StageObject.h"
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Treasure/Treasure.h"
#include "Game/GameObjects/Prop/Building/Building.h"
#include "Game/GameObjects/Helicopter/EscapeHelicopter/EscapeHelicopter.h"
#include "Game/Manager/BuildingManager/BuildingManager.h"

// ゲームオブジェクト管理系
#include "Game/Common/GameObjectManager/EnemyManager/EnemyManager.h"
#include "Game/Common/SpawnManager/SpawnManager.h"
#include "Game/Manager/PlayerManager/PlayerManager.h"

#include "Game/Common/Camera/PlayerCamera/PlayerCamera.h"
#include "Game/GameObjects/Player/PlayerController/PlayerController.h"

#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

#include "Library/ImaseLib/DebugFont.h"
#include "Game//Common/GameObjectRegistry/GameObjectRegistry.h"
#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/TaskManager/TaskManager.h"
#include <random>


// ロープオブジェクト

#include "Game/Common/ResultData/ResultData.h"

using namespace DirectX;




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pSceneManager   シーンを管理しているマネージャ
 */
StageManager::StageManager(const CommonResources* pCommonResources)
	: m_pCommonResources{ pCommonResources }
	, m_isStoppingUpdate{ false }
{

}



/**
 * @brief デストラクタ
 */
StageManager::~StageManager()
{
}


/**
 * @brief 初期化処理
 *
 * @param[in] screenWidth		画面の横幅
 * @param[in] screenHeight		画面の縦幅
 * * @return なし
 */
void StageManager::Initialize()
{
	using namespace SimpleMath;

}


/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool StageManager::UpdateTask(float deltaTime)
{
	if (m_isStoppingUpdate) { return true; }
	UpdateInGameObjects(deltaTime);
	return true;
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageManager::DrawTask(const Camera& camera)
{
	DrawInGameObjects(camera);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageManager::Finalize()
{

}

/**
 * @brief ウインドウサイズに依存するリソースを作成する
 */
void StageManager::CreateWindowSizeDependentResources()
{
	// ウィンドウサイズの取得
	auto windowSize = m_pCommonResources->GetDeviceResources()->GetOutputSize();
	float width = static_cast<float>(windowSize.right);
	float height = static_cast<float>(windowSize.bottom);

	// プレイヤーカメラの作成
	m_playerCamera = std::make_unique<PlayerCamera>(static_cast<int>(width), static_cast<int>(height), m_pCommonResources->GetMouseTracker());
}

/**
 * @brief インゲームのオブジェクトを更新する
 * * @param[in] deltaTime 経過時間
 */
void StageManager::UpdateInGameObjects(float deltaTime)
{
}

/**
 * @brief インゲームのオブジェクトを描画する
 * */
void StageManager::DrawInGameObjects(const Camera& camera)
{
	if (m_pCommonResources == nullptr) return;

	
	// スカイボックスの描画処理
	m_skySphere.UpdateEffects([](IEffect* effect)
		{
			auto lights = dynamic_cast<IEffectLights*>(effect);

			if (lights)
			{
				lights->SetLightEnabled(0, false);
				lights->SetLightEnabled(1, false);
				lights->SetLightEnabled(2, false);
			}

			auto basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetEmissiveColor(Colors::White);
			}
		});



	SimpleMath::Matrix world = SimpleMath::Matrix::CreateScale(SKYSPHERE_SCALE);
	world *= SimpleMath::Matrix::CreateTranslation(m_playerManager->GetPlayer()->GetPosition());

	m_skySphere.Draw(m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext(), *m_pCommonResources->GetCommonStates(), world, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}



// シーンの終了
void StageManager::OnEndScene()
{
	// ゲームオブジェクトの登録簿のクリア
	GameObjectRegistry::GetInstance()->Clear();
	// ゲームフロー通知の監視者の全削除
	GameFlowMessenger::GetInstance()->RemoveAllObserver();
	// 音管理の全削除
	SoundManager::GetInstance()->RemoveAll();

}

/**
 * @brief ステージの生成
 * */
void StageManager::CreateStage(CollisionManager* pCollisionManager, TaskManager* pTaskManager)
{
	using namespace SimpleMath;
	CreateWindowSizeDependentResources();


	// ----- 各種ゲームオブジェクトの作成 -------

	// **** 床の生成 *****
	m_floor = std::make_unique<Floor>();
	// 床の初期化
	m_floor->Initialize(SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_pCommonResources, pCollisionManager);

	// **** プレイヤーカメラの初期化処理 ****
	m_playerCamera->Initialize(m_pCommonResources, pCollisionManager);

	// メインカメラの設定 
	MainCamera::GetInstance()->SetCamera(m_playerCamera.get());

	// ***** プレイヤー管理の生成 *****
	m_playerManager = std::make_unique<PlayerManager>();
	m_playerManager->Initialize(m_pCommonResources, pCollisionManager, m_playerCamera.get());
	//// カメラにプレイヤーを設定する
	m_playerCamera->SetPlayer(m_playerManager->GetPlayer());


	// ***** 敵管理の作成 *****
	m_enemyManager = std::make_unique<EnemyManager>();
	m_enemyManager->Initialize();

	// ***** 出現管理の作成 *****
	m_spawnManager = std::make_unique<SpawnManager>();
	m_spawnManager->Initialize(m_enemyManager.get(), &m_escapeHelicopter, m_pCommonResources, pCollisionManager);

	// **** 天球の作成 ****
	m_skySphere = m_pCommonResources->GetResourceManager()->CreateModel("skyDome.sdkmesh");

	// ハードウェア乱数源からシードを生成
	static std::random_device seed_gen;

	// シードを使って乱数エンジンを初期化
	std::mt19937 engine(seed_gen());

	std::vector<Vector3> randomPosition =
	{
		TREASURE_POS_CANDIDATE_1,
		TREASURE_POS_CANDIDATE_2,
		TREASURE_POS_CANDIDATE_3,
	};
	std::shuffle(randomPosition.begin(), randomPosition.end(), engine);
	auto& treasurePosition = randomPosition.front();

	// お宝の生成
	m_treasure = std::make_unique<Treasure>();
	m_treasure->SetPosition(treasurePosition);
	m_treasure->Initialize(m_pCommonResources, pCollisionManager);

	m_buildingManager = std::make_unique<BuildingManager>();
	m_buildingManager->Initialize();
	m_buildingManager->RequestCreate(pCollisionManager, m_pCommonResources);
	// m_buildingManager->Save();

	// タスクの追加
	AddTask(pTaskManager);

}

/**
 * @brief タスクの追加
 * 
 * @param[in] pTaskManager　タスク管理
 */
void StageManager::AddTask(TaskManager* pTaskManager)
{
	// 追加するタスク
	std::vector<Task*> addTasks =
	{
		m_playerCamera	.get(),
		m_spawnManager	.get(),
		m_floor			.get(),
		m_playerManager	.get(),
		m_buildingManager.get(),
		m_enemyManager	.get(),
		m_treasure		.get()
	};
	for (auto& helicopter : m_escapeHelicopter)
	{
		addTasks.push_back(helicopter.get());
	}

	// 追加
	for (auto& task : addTasks)
	{
		pTaskManager->AddTask(task);
		// 親の設定
		task->ChangeParent(this);
	}
}

/**
 * @brief 更新処理を止める
 */
void StageManager::StopUpdating()
{
	m_isStoppingUpdate = true;
	DisableTask();
}

/**
 * @brief 更新処理を開始する
  */
void StageManager::StartUpdating()
{
	m_isStoppingUpdate = false;
	EnableTask();
}
