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

#include <fstream>
#include <random>


// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/DirectXFramework/DeviceResources.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/SoundManager/SoundManager.h"
#include "Game/Common/Framework/GameObjectManager/EnemyManager/EnemyManager.h"
#include "Game//Common/Framework/GameObjectRegistry/GameObjectRegistry.h"
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Framework/TaskManager/TaskManager.h"
#include "Game/Common/Framework/Collision/CollisionManager/CollisionManager.h"


// グラフィック関連
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Graphics/Shadow/CircularShadow/CircularShadow.h"
#include "Game/Common/Graphics/Camera/PlayerCamera/PlayerCamera.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/ResultData/ResultData.h"
#include "Game/Common/GameplayLogic/SpawnManager/SpawnManager.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Floor/Floor.h"
#include "Game/GameObjects/Wall/Wall.h"		// 壁
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/StageObject/StageObject.h"
#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Treasure/Treasure.h"
#include "Game/GameObjects/Prop/Building/Building.h"
#include "Game/GameObjects/Helicopter/EscapeHelicopter/EscapeHelicopter.h"
#include "Game/GameObjects/Player/PlayerController/PlayerController.h"

// ゲームオブジェクト管理関連
#include "Game/Common/Framework/GameObjectManager/PlayerManager/PlayerManager.h"
#include "Game/Common/Framework/GameObjectManager/CheckpointManager/CheckpointManager.h"
#include "Game/Common/Framework/GameObjectManager/BuildingManager/BuildingManager.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

// シーン関連
#include "Game/Scene/TitleScene/TitleScene.h"
#include "Game/Scene/ResultScene/ResultScene.h"
#include "Game/Scene/Loading/LoadingScreen.h"




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
	, m_pSpawnManager	{ nullptr }
{
	GameFlowMessenger::GetInstance()->RegistryObserver(this);
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
void StageManager::Initialize(SpawnManager* pSpawnManager, CollisionManager* pCollisionManager, TaskManager* pTaskManager)
{
	CreateWindowSizeDependentResources();

	// メインカメラの作成
	MainCamera::GetInstance()->SetCamera(m_playerCamera.get());

	// 出現管理の設定
	m_pSpawnManager = pSpawnManager;

	// ****プレイヤー管理の作成 * ***
	m_playerManager = std::make_unique<PlayerManager>();
	m_playerManager->Initialize(m_pCommonResources,  m_playerCamera.get());

	// ----- 各種ゲームオブジェクトの作成 -------
	m_buildingManager = std::make_unique<BuildingManager>(m_pCommonResources);
	m_buildingManager->Initialize();

	// **** 床の生成 *****
	m_floor = std::make_unique<Floor>();
	// 床の初期化
	m_floor->Initialize(SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_pCommonResources, pCollisionManager);

	// **** プレイヤーカメラの初期化処理 ****
	m_playerCamera->Initialize(m_pCommonResources, pCollisionManager);

	// ***** 敵管理の作成 *****
	m_enemyManager = std::make_unique<EnemyManager>();
	m_enemyManager->Initialize();
	// **** チェックポイント管理 の作成 ****
	m_checkpointManager = std::make_unique<CheckpointManager>();

	// タスクの追加
	AddTask(pTaskManager);

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
	UNREFERENCED_PARAMETER(deltaTime);
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
	world *= SimpleMath::Matrix::CreateTranslation(m_playerManager->GetPlayer()->GetTransform()->GetPosition());

	m_skySphere.Draw(m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext(), *m_pCommonResources->GetCommonStates(), world, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}

void StageManager::OnGameFlowEvent(GameFlowEventID eventID)
{
	UNREFERENCED_PARAMETER(eventID);
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

void StageManager::CreateCheckpoint(CollisionManager* pCollisionManager)
{
	const Building* tileBuilding = nullptr;
	if (m_buildingManager->FindBuilding(11, tileBuilding))
	{
		m_checkpointManager->CreateCheckpoint(tileBuilding->GetTransform()->GetPosition() + SimpleMath::Vector3(0.0f, tileBuilding->GetExtent().y, 0.0f), pCollisionManager, m_pCommonResources);
	}
}

/**
 * @brief ステージの生成
 * */
void StageManager::CreateStage(CollisionManager* pCollisionManager)
{
	using namespace SimpleMath;
	if (m_pSpawnManager == nullptr) {return;}

	// ***** 最初のステージレイアウトの作成 *****
	m_pSpawnManager->SetManagers(m_playerManager.get(), m_buildingManager.get(), m_enemyManager.get(), &m_escapeHelicopter, m_playerCamera.get());

	// ステージの作成
	m_pSpawnManager->SetupInitialLayout();
	// チェックポイントの作成 *** 今後Manager化
	CreateCheckpoint(pCollisionManager);


	// プレイヤーカメラにプレイヤーを設定
	m_playerCamera->SetPlayer(m_playerManager->GetPlayer());
	MainCamera::GetInstance()->SetCamera(m_playerCamera.get());

	// **** 天球の作成 ****
	m_skySphere = m_pCommonResources->GetResourceManager()->CreateModel("skyDome.sdkmesh");
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
		m_floor			.get(),
		m_playerManager	.get(),
		m_buildingManager.get(),
		m_enemyManager	.get(),
		m_treasure		.get(),
		m_checkpointManager.get()
	};
	for (auto& helicopter : m_escapeHelicopter)
	{
		
		addTasks.push_back(helicopter.get());
	}

	// 追加
	for (auto& task : addTasks)
	{
		if (task == nullptr) { continue; }
		pTaskManager->AddTask(task);
		// 親の設定
		task->ChangeParent(this);
	}

}

void StageManager::RemoveTask(TaskManager* pTaskManager)
{
	// 削除するタスク
	std::vector<Task*> removeTasks =
	{
		m_floor.get(),
		m_playerManager.get(),
		m_buildingManager.get(),
		m_enemyManager.get(),
		m_treasure.get(),
		m_checkpointManager.get()
	};
	for (auto& helicopter : m_escapeHelicopter)
	{
		removeTasks.push_back(helicopter.get());
	}
	// 追加
	for (auto& task : removeTasks)
	{
		if (task == nullptr) { continue; }
		pTaskManager->DeleteTask(task);
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
