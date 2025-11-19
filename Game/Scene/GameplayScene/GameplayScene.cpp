/**
 * @file   GameplayScene.cpp
 *
 * @brief  ゲームプレイシーンに関するソースファイル
 *
 * @author 松下大暉
 *
 * @date   2025/03/29
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "GameplayScene.h"

// ライブラリ
#include "Game/Common/TaskManager/TaskManager.h"

// DirectX系
#include "Game/Common/DeviceResources.h"

// ゲームデータ
#include "Game/Common/ResultData/ResultData.h"
#include "Game/Common/CommonResources/CommonResources.h"

// 基盤システム
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/Common/SoundManager/SoundManager.h"
#include "Game/Common/SoundManager/SoundPaths.h"
#include "Game/Scene/Loading/LoadingScreen.h"
#include "Game/Common/GameEffect/GameEffectController.h"


// 管理系
#include "Game//Common/GameObjectRegistry/GameObjectRegistry.h"
#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Manager/StageManager/StageManager.h"

// 状態
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PoseGameplayState.h"

// シーン
#include "Game/Scene/ResultScene/ResultScene.h"

using namespace DirectX;




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pSceneManager   シーンを管理しているマネージャ
 */
GameplayScene::GameplayScene()
{
	GameObjectRegistry::GetInstance()->Clear();

	GameFlowMessenger::GetInstance()->RemoveAllObserver();
	GameFlowMessenger::GetInstance()->RegistrObserver(this);
}



/**
 * @brief デストラクタ
 */
GameplayScene::~GameplayScene()
{
}


/**
 * @brief 初期化処理
 *
 * * @return なし
 */
void GameplayScene::Initialize()
{
	// 画面サイズに関するモノの作成
	CreateWindowSizeDependentResources();

	// ゲームプレイ開始時のセットアップ
	SetUpForGameStart();

	// 基盤の作成
	CreatePlatform();

	// タスクの作成
	CreateTask();

	// ステージ作成 
	CreateStage();

	// ゲーム開始
	StartGame();
}


/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void GameplayScene::Update(float deltaTime)
{
	// ゲームプレイ時間の加算
	m_gamePlayingTimeCounter.UpperTime(deltaTime);
	// 状態の更新処理
	m_stateMachine->Update(deltaTime);
	// イベントの解消
	ResolveEvents();
}

/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::Render()
{
	// 状態の描画処理
	m_stateMachine->Draw();
}

/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::Finalize()
{
	m_stateMachine->ClearState();
}

/**
 * @brief イベントの解消
 * 
 */
void GameplayScene::ResolveEvents()
{
	// イベントスタックの解消(仮実装)
	for (auto& event : m_eventStack)
	{
		event();
	}
	m_eventStack.clear();
}



/**
 * @brief イベントメッセージを受け取る
 * * @param[in] eventID
 */
void GameplayScene::OnGameFlowEvent(GameFlowEventID eventID)
{
	switch (eventID)
	{
	case GameFlowEventID::STOLE_TREASURE:
		break;
	case GameFlowEventID::PLAYER_DIE:
		m_eventStack.emplace_back([&]() {
			OnEndScene();
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetElapsedTime(), false);
			ChangeScene<ResultScene, LoadingScreen>();
			});
		break;
	case GameFlowEventID::ESCAPE_SUCCESS:
		m_eventStack.emplace_back([&]() {
			OnEndScene();
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetElapsedTime(), true);
			ChangeScene<ResultScene, LoadingScreen>();
			});
		break;
	default:
		break;
	}
}


// シーンの終了
void GameplayScene::OnEndScene()
{
	// ゲームオブジェクトの登録簿のクリア
	GameObjectRegistry::GetInstance()->Clear();
	// ゲームフロー通知の監視者の全削除
	GameFlowMessenger::GetInstance()->RemoveAllObserver();
	// 音管理の全削除
	SoundManager::GetInstance()->RemoveAll();

}


// --------------------------------------------------------------------
// 初期設定関連 
// --------------------------------------------------------------------

/**
* @brief ウインドウサイズに依存するリソースを作成する
*/
void GameplayScene::CreateWindowSizeDependentResources()
{
	// ウィンドウサイズの取得
	auto windowSize = GetCommonResources()->GetDeviceResources()->GetOutputSize();
	float width = static_cast<float>(windowSize.right);
	float height = static_cast<float>(windowSize.bottom);
}

/**
 * @brief ゲーム開始時のセットアップ
 * 
 */
void GameplayScene::SetUpForGameStart()
{
	// **** ゲーム経過時間のリセット *****
	m_gamePlayingTimeCounter.Reset();

	// リザルトデータの初期化処理
	ResultData::GetInstance()->Reset();
}

/**
 * @brief 基盤の作成
 */
void GameplayScene::CreatePlatform()
{
	// **** 衝突管理の生成 ****
	m_collisionManager = std::make_unique<CollisionManager>();

	// **** エフェクト管理の作成 ****
	m_gameEffectManager = std::make_unique<GameEffectManager>();
	// 現在使用するエフェクト管理の取得
	GameEffectController::GetInstance()->SetGameEffectManager(m_gameEffectManager.get());

	// **** ステートマシーンの作成 ****
	m_stateMachine = std::make_unique <StateMachine<GameplayScene>>(this);
	// **** タスク管理の作成 ****
	m_taskManager = std::make_unique<TaskManager>();

	// ステージ管理の作成
	m_stageManager = std::make_unique<StageManager>(GetCommonResources());
}

/**
 * @brief ステージの生成
 * */
void GameplayScene::CreateStage()
{
	
	m_stageManager->Initialize();
	m_stageManager->CreateStage(m_collisionManager.get(), m_taskManager.get());

}

/**
 * @brief タスクの作成
 * 
 */
void GameplayScene::CreateTask()
{
	
	// **** タスクの登録 ****
	m_taskManager->AddTask(m_collisionManager.get());	// CollisionManager
	m_taskManager->AddTask(m_stageManager.get());		// StageManager
	m_taskManager->AddTask(m_gameEffectManager.get());	// EffectManager
}

/**
 * @brief ゲームを開始する
 */
void GameplayScene::StartGame()
{
	// **** 最初の状態 ****
	m_stateMachine->ChangeState<NormalGameplayState>();
	// **** BGMを鳴らす ****
	SoundManager::GetInstance()->RemoveAll();
	SoundManager::GetInstance()->Play(SoundPaths::BGM_INGAME, true);


	// ***** ゲーム開始通知 *****
	GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_START);
}
