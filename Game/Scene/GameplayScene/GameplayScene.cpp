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
#include <random>

#include "Game/Manager/StageManager/StageManager.h"

// 状態
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PoseGameplayState.h"

// ロープオブジェクト

#include "Game/Common/ResultData/ResultData.h"

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

#include "Game/Common/GameEffect/GameEffectController.h"
#include "Game/Common/GameEffect/Effects/SimpleParticle/SimpleParticle.h"

/**
 * @brief 初期化処理
 *
 * @param[in] screenWidth		画面の横幅
 * @param[in] screenHeight		画面の縦幅
 * * @return なし
 */
void GameplayScene::Initialize()
{
	using namespace SimpleMath;
	CreateWindowSizeDependentResources();

	// 処理に使用するものたちの取得
	ID3D11DeviceContext* context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();

	// **** ステートマシーンの作成 ****
	m_stateMachine = std::make_unique <StateMachine<GameplayScene>>(this);
	// **** 最初の状態 ****
	m_stateMachine->ChangeState<NormalGameplayState>();

	// **** ゲーム経過時間のリセット *****
	m_gamePlayingTimeCounter.Reset();

	// リザルトデータの初期化処理
	ResultData::GetInstance()->Reset();

	// **** 衝突管理の生成 ****
	m_collisionManager = std::make_unique<CollisionManager>();


	// **** UI関連処理 後にクラス化して分離する ****
	m_canvas = std::make_unique<Canvas>();
	// キャンバスの初期化処理
	m_canvas->Initialize(context);

	// **** エフェクト管理の作成 ****
	m_gameEffectManager = std::make_unique<GameEffectManager>();
	// 現在使用するエフェクト管理の取得
	GameEffectController::GetInstance()->SetGameEffectManager(m_gameEffectManager.get());

	// **** ステージの作成 ****
	CreateStage();


	// **** BGMを鳴らす ****
	SoundManager::GetInstance()->RemoveAll();
	SoundManager::GetInstance()->Play(SoundPaths::BGM_INGAME, true);


	// ***** ゲーム開始通知 *****
	GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_START);
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
	// 状態の更新処理
	m_stateMachine->Update(deltaTime);


	// イベントスタックの解消(仮実装)
	for (auto& event : m_eventStack)
	{
		event();
	}
	m_eventStack.clear();

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
 * @brief ウインドウサイズに依存するリソースを作成する
 */
void GameplayScene::CreateWindowSizeDependentResources()
{
	// ウィンドウサイズの取得
	auto windowSize = GetCommonResources()->GetDeviceResources()->GetOutputSize();
	float width = static_cast<float>(windowSize.right);
	float height = static_cast<float>(windowSize.bottom);

	// デバッグカメラの作成
	m_debugCamera = std::make_unique<Imase::DebugCamera>(static_cast<int>(width), static_cast<int>(height));

	// 射影行列の作成
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(CAMERA_FOV_DEGREES)
		, static_cast<float>(width) / static_cast<float>(height)
		, CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP); // farを少し伸ばす
}

/**
 * @brief インゲームのオブジェクトを更新する
 * * @param[in] deltaTime 経過時間
 */
void GameplayScene::UpdateInGameObjects(float deltaTime)
{
	auto kb = Keyboard::Get().GetState();

	// ゲームプレイ時間の加算
	m_gamePlayingTimeCounter.UpperTime(deltaTime);

	// ゲームエフェクト管理の更新処理
	m_gameEffectManager->Update(deltaTime);

	// デバックカメラの更新
	m_debugCamera->Update();

	auto mouse = Mouse::Get().GetState();

	// 衝突管理の更新処理
	m_collisionManager->Update();

	m_stageManager->UpdateInGameObjects(deltaTime);

}

/**
 * @brief インゲームのオブジェクトを描画する
 * */
void GameplayScene::DrawInGameObjects()
{
	if (GetCommonResources() == nullptr) return;



	// デバッグカメラからビュー行列を取得する
	SimpleMath::Matrix view = MainCamera::GetInstance()->GetCamera()->GetView();
	GameEffectController::GetInstance()->SetView(view);
	GameEffectController::GetInstance()->SetProjection(m_projection);

	// 共通リソース
	auto states = GetCommonResources()->GetCommonStates();

	m_stageManager->DrawInGameObjects(view, m_projection);
	
	// ゲームエフェクト管理の描画処理
	m_gameEffectManager->Draw(view, m_projection);



	m_canvas->Draw(states);
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
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetdeltaTime(), false);
			ChangeScene<ResultScene, LoadingScreen>();
			});
		break;
	case GameFlowEventID::ESCAPE_SUCCESS:
		m_eventStack.emplace_back([&]() {
			OnEndScene();
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetdeltaTime(), true);
			ChangeScene<ResultScene, LoadingScreen>();
			});
		break;
	default:
		break;
	}
}

/**
 * @brief キャンバス
 * * @return キャンバス
 */
Canvas* GameplayScene::GetCanvas() const
{
	return m_canvas.get();
}

/**
 * @brief ステージ管理の取得
 * 
 * @return ステージ管理
 */
StageManager* GameplayScene::GetStageManager() const
{
	return m_stageManager.get();
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

/**
 * @brief ステージの生成
 * */
void GameplayScene::CreateStage()
{
	// ステージ管理の作成
	m_stageManager = std::make_unique<StageManager>(GetCommonResources());
	
	m_stageManager->Initialize();
	m_stageManager->CreateStage(m_collisionManager.get());

}
