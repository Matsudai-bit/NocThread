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
{
	GameObjectRegistry::GetInstance()->Clear();

	GameFlowMessenger::GetInstance()->RemoveAllObserver();
	GameFlowMessenger::GetInstance()->RegistrObserver(this);
}



/**
 * @brief デストラクタ
 */
StageManager::~StageManager()
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
void StageManager::Initialize()
{
	using namespace SimpleMath;
	CreateWindowSizeDependentResources();

	// 処理に使用するものたちの取得
	ID3D11DeviceContext* context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	// **** ゲーム経過時間のリセット *****
	m_gamePlayingTimeCounter.Reset();


	// リザルトデータの初期化処理
	ResultData::GetInstance()->Reset();


	// **** BGMを鳴らす ****
	SoundManager::GetInstance()->RemoveAll();
	SoundManager::GetInstance()->Play(SoundPaths::BGM_INGAME, true);


	// **** 衝突管理の生成 ****
	m_collisionManager = std::make_unique<CollisionManager>();


	// **** UI関連処理 後にクラス化して分離する ****
	m_canvas = std::make_unique<Canvas>();
	// キャンバスの初期化処理
	m_canvas->Initialize(context);
	// **** 画面中央スプライトの生成 ****

	// 今は使わない
	//m_scopeSprite = std::make_unique<Sprite>();
	//// スプライトの初期化処理
	//m_scopeSprite->Initialize(m_pCommonResources->GetResourceManager()->CreateTexture(SCOPE_TEXTURE_PATH));
	//m_scopeSprite->SetPosition(DirectX::SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF() - SCOPE_Y_OFFSET * Screen::Get()->GetScreenScale()));
	//m_scopeSprite->SetScale(SCOPE_SCALE * Screen::Get()->GetScreenScale());
	//	m_canvas->AddSprite(m_scopeSprite.get());


		// **** エフェクト官吏の作成 ****
	m_gameEffectManager = std::make_unique<GameEffectManager>();
	// 現在使用するエフェクト管理の取得
	GameEffectController::GetInstance()->SetGameEffectManager(m_gameEffectManager.get());
	// メインカメラの設定 
	MainCamera::GetInstance()->SetCamera(m_playerCamera.get());


	

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
void StageManager::Update(float deltaTime)
{


}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageManager::Render(DirectX::SimpleMath::Matrix& proj, DirectX::SimpleMath::Matrix& view)
{

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

	// デバッグカメラの作成
	m_debugCamera = std::make_unique<Imase::DebugCamera>(static_cast<int>(width), static_cast<int>(height));

	// プレイヤーカメラの作成
	m_playerCamera = std::make_unique<PlayerCamera>(static_cast<int>(width), static_cast<int>(height), m_pCommonResources->GetMouseTracker());

	// 射影行列の作成
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(CAMERA_FOV_DEGREES)
		, static_cast<float>(width) / static_cast<float>(height)
		, CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP); // farを少し伸ばす
}

/**
 * @brief インゲームのオブジェクトを更新する
 * * @param[in] deltaTime 経過時間
 */
void StageManager::UpdateInGameObjects(float deltaTime)
{
	auto kb = Keyboard::Get().GetState();

	// ゲームプレイ時間の加算
	m_gamePlayingTimeCounter.UpperTime(deltaTime);

	// ゲームエフェクト管理の更新処理
	m_gameEffectManager->Update(deltaTime);

	// 出現管理の更新処理
	m_spawnManager->Update(deltaTime);

	// デバックカメラの更新
	m_debugCamera->Update();

	auto mouse = Mouse::Get().GetState();

	m_playerCamera->Update(deltaTime);

	// 衝突管理の更新処理
	m_collisionManager->Update();

	m_playerManager->Update(deltaTime, m_proj);

	for (auto& stageObj : m_stageObject)
	{
		stageObj->Update(deltaTime);
	}

	// お宝の更新処理
	m_treasure->Update(deltaTime);

	// 敵の更新処理
	m_enemyManager->Update(deltaTime);

	// 脱出用ヘリコプターの更新処理
	for (auto& helicopter : m_escapeHelicopter)
	{
		helicopter->Update(deltaTime);
	}




}

/**
 * @brief インゲームのオブジェクトを描画する
 * */
void StageManager::DrawInGameObjects()
{
	if (m_pCommonResources == nullptr) return;



	// デバッグカメラからビュー行列を取得する
	SimpleMath::Matrix view = MainCamera::GetInstance()->GetCamera()->GetView();
	GameEffectController::GetInstance()->SetView(view);
	GameEffectController::GetInstance()->SetProjection(m_proj);

	// 共通リソース
	auto states = m_pCommonResources->GetCommonStates();

	// グリッド床の描画
	//m_gridFloor->Render(context, view, m_proj);

	m_floor->Draw(view, m_proj);

	// プレイヤーの描画処理
	m_playerManager->Draw(view, m_proj);


	for (auto& stageObj : m_stageObject)
	{
		stageObj->Draw(view, m_proj);
	}

	// 建物の描画処理
	m_buildingManager->Draw(view, m_proj);

	// 脱出用ヘリコプターの描画処理
	for (auto& helicopter : m_escapeHelicopter)
	{
		helicopter->Draw(view, m_proj);
	}


	// 敵管理の描画処理
	m_enemyManager->Draw(view, m_proj);

	// お宝の描画処理
	m_treasure->Draw(view, m_proj);

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
				basicEffect->SetEmissiveColor(Colors::White);


		});



	SimpleMath::Matrix world = SimpleMath::Matrix::CreateScale(SKYSPHERE_SCALE);
	world *= SimpleMath::Matrix::CreateTranslation(m_playerManager->GetPlayer()->GetPosition());

	m_skySphere.Draw(m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext(), *m_pCommonResources->GetCommonStates(), world, view, m_proj);
	// ゲームエフェクト管理の描画処理
	m_gameEffectManager->Draw(view, m_proj);



	m_canvas->Draw(states);
}

/**
 * @brief イベントメッセージを受け取る
 * * @param[in] eventID
 */
void StageManager::OnGameFlowEvent(GameFlowEventID eventID)
{
	switch (eventID)
	{
	case GameFlowEventID::STOLE_TREASURE:
		break;
	case GameFlowEventID::PLAYER_DIE:
		m_eventStack.emplace_back([&]() {
			OnEndScene();
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetdeltaTime(), false);
			});
		break;
	case GameFlowEventID::ESCAPE_SUCCESS:
		m_eventStack.emplace_back([&]() {
			OnEndScene();
			ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetdeltaTime(), true);
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
Canvas* StageManager::GetCanvas() const
{
	return m_canvas.get();
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
void StageManager::CreateStage()
{
	using namespace SimpleMath;


	// ----- 各種ゲームオブジェクトの作成 -------

	// **** 床の生成 *****
	m_floor = std::make_unique<Floor>();
	// 床の初期化
	m_floor->Initialize(SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_pCommonResources, m_collisionManager.get());

	// **** プレイヤーカメラの初期化処理 ****
	m_playerCamera->Initialize(m_pCommonResources, m_collisionManager.get());

	// ***** プレイヤー管理の生成 *****
	m_playerManager = std::make_unique<PlayerManager>();
	m_playerManager->Initialize(m_pCommonResources, m_collisionManager.get(), m_playerCamera.get());
	//// カメラにプレイヤーを設定する
	m_playerCamera->SetPlayer(m_playerManager->GetPlayer());


	// ***** 敵管理の作成 *****
	m_enemyManager = std::make_unique<EnemyManager>();
	m_enemyManager->Initialize();

	// ***** 出現管理の作成 *****
	m_spawnManager = std::make_unique<SpawnManager>();
	m_spawnManager->Initialize(m_enemyManager.get(), &m_escapeHelicopter, m_pCommonResources, m_collisionManager.get());

	// ***** ステージ作成 *****
	//CreateStage();

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
	m_treasure->Initialize(m_pCommonResources, m_collisionManager.get());

	m_buildingManager = std::make_unique<BuildingManager>();
	m_buildingManager->Initialize();
	m_buildingManager->RequestCreate(m_collisionManager.get(), m_pCommonResources);
	// m_buildingManager->Save();
	// ****** ここまで ******************************************************



}
