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
#include "Library/DirectXFramework/DeviceResources.h"

// ファクトリー関連
#include "Game/Common/Factory/CollisionMatrixFactory/CollisionMatrixFactory.h"

// データベース関連
#include "Game/Common/Database/SoundDatabase.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/SoundManager/SoundManager.h"
#include "Game/Common/Framework/TaskManager/TaskManager.h"
#include "Game//Common/Framework/GameObjectRegistry/GameObjectRegistry.h"
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/ResultData/ResultData.h"
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"
#include "Game/Common/GameplayLogic/StageManager/StageManager.h"
#include "Game/Common/GameplayLogic/SpawnManager/SpawnManager.h"
#include "Game/Common/GameplayLogic/GameDirector/GameDirector.h"
#include "Game/Common/GameplayLogic/CollisionMatrix/CollisionMatrix.h"

// グラフィック関連
#include "Game/Common/Graphics/GameEffect/GameEffectController.h"
#include "Game/Common/Graphics/TransitionMask/TransitionMask.h"
#include "Game/Common/Graphics/Camera/PlayerCamera/PlayerCamera.h"
#include "Game/Common/Graphics/GameEffect/Effects/ConcentrationLines/ConcentrationLines.h"

// ミニマップ関連
#include "Game/Common/MiniMap/MiniMap.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

// プレイシーンの状態関連
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PauseGameplayState.h"
#include "Game/Scene/GameplayScene/State/StartingGameplayState/StartingGameplayState.h"

// シーン関連
#include "Game/Scene/ResultScene/ResultScene.h"
#include "Game/Scene/Loading/LoadingScreen.h"

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
	GameFlowMessenger::GetInstance()->RegistryObserver(this);
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
	std::vector <std::function<void()>> copy = m_eventStack;

	m_eventStack.clear();
	// イベントスタックの解消(仮実装)
	for (auto& event : copy)
	{
		event();
	}
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

			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetElapsedTime(), false);
			GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_END);

		});
	
		break;
	case GameFlowEventID::ESCAPE_SUCCESS:
		m_eventStack.emplace_back([&]() {
				
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetElapsedTime(), true);
			GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_END);
				
		});
		break;
	case GameFlowEventID::GAME_END:
		// シーンを切り替える
		m_eventStack.emplace_back([&]()
		{

			GetCommonResources()->GetTransitionMask()->Close([&]()
				{
					OnEndScene();
					ChangeScene<ResultScene, LoadingScreen>();

				});
		});
		break;
	case GameFlowEventID::GAME_SETUP_FINISH:
		GetCommonResources()->GetCollisionManager()->CreateStaticProxy();
	default:
		break;
	}
}


// シーンの終了
void GameplayScene::OnEndScene()
{
	GetCommonResources()->GetCollisionManager()->RemoveAll();
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
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetCommonResources()->GetCommonStates();

	// ステートマシーンの作成
	m_stateMachine = std::make_unique <StateMachine<GameplayScene>>(this);

	// ゲーム進行の監督の作成
	m_gameDirector = std::make_unique<GameDirector>();
	// 衝突検知表の生成
	m_collisionMatrix = CollisionMatrixFactory::StageCollisionMatrix().Create(DefaultSpawnDesc());

	// エフェクト管理の作成
	m_gameEffectManager = std::make_unique<GameEffectManager>();

	// タスク管理の作成
	m_taskManager = std::make_unique<TaskManager>();

	// オブジェクト出現管理の作成
	m_spawnManager = std::make_unique<SpawnManager>();

	// ステージ管理の作成
	m_stageManager = std::make_unique<StageManager>(GetCommonResources());

	// ミニマップの作成
	m_miniMap = std::make_unique<Minimap>(GetCommonResources());

	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>(context, states);

	// 基盤のセットアップ
	SetupPlatform();
}

/**
 * @brief 基盤のセットアップ
 * 
 */
void GameplayScene::SetupPlatform()
{
	// ステージの初期化処理
	m_stageManager->Initialize(m_spawnManager.get(), GetCommonResources()->GetCollisionManager(), m_taskManager.get());

	auto collisionManager = GetCommonResources()->GetCollisionManager();

	// 衝突管理に衝突検知表を設定
	collisionManager->SetCollisionMatrix(m_collisionMatrix.get());

	// ゲームディレクターの初期化処理
	m_gameDirector->Initialize(GetCommonResources());

	// 現在使用するエフェクト管理の取得
	GameEffectController::GetInstance()->SetGameEffectManager(m_gameEffectManager.get());
	// 出現管理の初期化処理
	m_spawnManager->Initialize(GetCommonResources(), GetCommonResources()->GetCollisionManager());

	// 一番手前にする
	m_canvas->SetOt(0);
}

/**
 * @brief ステージの生成
 * */
void GameplayScene::CreateStage()
{
	m_stageManager->CreateStage(GetCommonResources()->GetCollisionManager());
}

/**
 * @brief タスクの作成
 * 
 */
void GameplayScene::CreateTask()
{
	// **** タスクの登録 ****
	m_taskManager->AddTask(m_gameDirector.get());		// GameDirector
	m_taskManager->AddTask(m_stageManager.get());		// StageManager
	m_taskManager->AddTask(m_spawnManager.get());		// SpawnManager
	m_taskManager->AddTask(m_stageManager->GetPlayerCamera());		// SpawnManager
	m_taskManager->AddTask(m_gameEffectManager.get());	// EffectManager
	m_taskManager->AddTask(m_miniMap.get());			// Minimap
	m_taskManager->AddTask(m_canvas.get());				// Canvas
}

/**
 * @brief ゲームを開始する
 */
void GameplayScene::StartGame()
{
	// トランジションマスクをオープンする
	GetCommonResources()->GetTransitionMask()->Open();


	// **** 最初の状態 ****
	m_stateMachine->ChangeState<StartingGameplayState>();
	// **** BGMを鳴らす ****
	SoundManager::GetInstance()->RemoveAll();
	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::BGM_INGAME), true);

	// ***** ゲーム開始通知 *****
	GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_SETUP_FINISH);

}
