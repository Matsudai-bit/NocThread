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


#include "Game/Common/Camera/PlayerCamera/PlayerCamera.h"
#include "Game/GameObjects/Player/PlayerController/PlayerController.h"

#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

#include "Library/ImaseLib/DebugFont.h"
#include "Game//Common/GameObjectRegistry/GameObjectRegistry.h"
#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include <random>

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
 * @param[in] pSceneManager    シーンを管理しているマネージャ
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
 * 
 * @return なし
 */
void GameplayScene::Initialize()
{
    using namespace SimpleMath;
    CreateWindowSizeDependentResources();

    // 処理に使用するものたちの取得
    DX::DeviceResources*    pDeviceResources    = GetCommonResources()->GetDeviceResources();
    CommonStates*           pStates             = GetCommonResources()->GetCommonStates();
    ID3D11DeviceContext*    context             = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();


    // **** ステートマシーンの作成 ****
    m_stateMachine = std::make_unique <StateMachine<GameplayScene>>(this);
    // **** 最初の状態 ****
    m_stateMachine->ChangeState<NormalGameplayState>();


    // **** ゲーム経過時間のリセット *****
    m_gamePlayingTimeCounter.Reset();


    // リザルトデータの初期化処理
    ResultData::GetInstance()->Reset();

    
    // **** BGMを鳴らす ****
    SoundManager::GetInstance()->RemoveAll();
    SoundManager::GetInstance()->Play(SoundPaths::BGM_INGAME, true);

    
    // **** 衝突管理の生成 ****
    m_collisionManager  =   std::make_unique<CollisionManager>();


    // **** UI関連処理 後にクラス化して分離する ****
    m_canvas = std::make_unique<Canvas>();
    // キャンバスの初期化処理
    m_canvas->Initialize(context);
    // **** 画面中央スプライトの生成 ****
    m_scopeSprite = std::make_unique<Sprite>();
    // スプライトの初期化処理
    m_scopeSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("scope.dds"));
    m_scopeSprite->SetPosition(DirectX::SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF() - 140.f *  Screen::Get()->GetScreenScale()));
    m_scopeSprite->SetScale(0.09f * Screen::Get()->GetScreenScale());
    m_canvas->AddSprite(m_scopeSprite.get());
 

    // **** エフェクト官吏の作成 ****
    m_gameEffectManager = std::make_unique<GameEffectManager>();
    // 現在使用するエフェクト管理の取得
    GameEffectController::GetInstance()->SetGameEffectManager(m_gameEffectManager.get());
    // メインカメラの設定 
    MainCamera::GetInstance()->SetCamera(m_playerCamera.get());


    // **** 各種ゲームオブジェクトの作成 *****    
    
    // 床の生成
    m_floor = std::make_unique<Floor>();
    // 床の初期化
    m_floor->Initialize(SimpleMath::Vector3(0.0f, 0.0f, 0.0f), GetCommonResources(), m_collisionManager.get());

    // ---- プレイヤー関連 後に管理クラスを作る ----------
    // プレイヤーカメラの初期化処理
    m_playerCamera->Initialize(GetCommonResources(), m_collisionManager.get());

    // プレイヤーの生成
    m_player = std::make_unique<Player>();
    // プレイヤーの初期化
    m_player->Initialize(GetCommonResources(), m_collisionManager.get(), m_playerCamera.get());

    // プレイヤー影の作成
    m_playerShadow = std::make_unique<CircularShadow>();
    m_playerShadow->Initialize(GetCommonResources()->GetDeviceResources(), 0.01f);
   
    // プレイヤーコントローラの作成
    m_playerController = std::make_unique<PlayerController>(m_player.get(), m_playerCamera.get());
    // カメラにプレイヤーを設定する
    m_playerCamera->SetPlayer(m_player.get());
    // ---------------------------- ここまで

    // 敵管理の作成
    m_enemyManager = std::make_unique<EnemyManager>();
    m_enemyManager->Initialize();

    // 出現管理の作成
    m_spawnManager = std::make_unique<SpawnManager>();
    m_spawnManager->Initialize(m_enemyManager.get(), &m_escapeHelicopter, GetCommonResources(), m_collisionManager.get());

    // ステージ作成
    CreateStage();
  
    // **** 天球の作成 ****
    m_skySphere = GetCommonResources()->GetResourceManager()->CreateModel("skyDome.sdkmesh");

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
}

/**
 * @brief ウインドウサイズに依存するリソースを作成する
 */
void GameplayScene::CreateWindowSizeDependentResources()
{
    // ウィンドウサイズの取得
    auto windowSize = GetCommonResources()->GetDeviceResources()->GetOutputSize();
    float width  = static_cast<float>(windowSize.right);
    float height = static_cast<float>(windowSize.bottom);

    // デバッグカメラの作成
    m_debugCamera = std::make_unique<Imase::DebugCamera>(width, height);

    // プレイヤーカメラの作成
    m_playerCamera = std::make_unique<PlayerCamera>(width, height, GetCommonResources()->GetMouseTracker());

    // 射影行列の作成
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(45.0f)
        , static_cast<float>(width) / static_cast<float>(height)
       , 0.1f, 320.0f); // farを少し伸ばす
}

/**
 * @brief インゲームのオブジェクトを更新する
 * 
 * @param[in] deltaTime 経過時間
 */
void GameplayScene::UpdateInGameObjects(float deltaTime)
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

    // プレイヤーコントローラの更新処理
    m_playerController->Update(deltaTime, GetCommonResources()->GetKeyboardTracker(), GetCommonResources()->GetMouseTracker());
    // プレイヤーの更新処理
    m_player->Update(deltaTime,  m_proj);

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
 * 
 */
void GameplayScene::DrawInGameObjects()
{
    if (GetCommonResources() == nullptr) return;



    // デバッグカメラからビュー行列を取得する
    SimpleMath::Matrix view = MainCamera::GetInstance()->GetCamera()->GetView();
    GameEffectController::GetInstance()->SetView(view);
    GameEffectController::GetInstance()->SetProjection(m_proj);

    // 共通リソース
    auto states = GetCommonResources()->GetCommonStates();

    // グリッド床の描画
    //m_gridFloor->Render(context, view, m_proj);

    m_floor->Draw(view, m_proj);

    // プレイヤーの描画処理
    m_player->Draw(view, m_proj);


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



    // 仮UI
    GetCommonResources()->GetDebugFont()->AddString(Screen::Get()->GetRight() - static_cast<int>(static_cast<float>(240 * Screen::Get()->GetScreenScale())), static_cast<int>(static_cast<float>(20) * Screen::Get()->GetScreenScale()), Colors::White, L"ESC : PAUSE MENU");


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



    SimpleMath::Matrix world = SimpleMath::Matrix::CreateScale(300.0f);
    world *= SimpleMath::Matrix::CreateTranslation(m_player->GetPosition());

    m_skySphere.Draw(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext(), *GetCommonResources()->GetCommonStates(), world, view, m_proj);
    // ゲームエフェクト管理の描画処理
    m_gameEffectManager->Draw(view, m_proj);
    //
    m_playerShadow->Draw(view, m_proj, states, m_player->GetPosition());


    m_canvas->Draw(states);
}

/**
 * @brief イベントメッセージを受け取る
 * 
 * @param[in] eventID
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
            ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetdeltaTime(),  true);
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

/**
 * @brief ステージの生成
 * 
 */
void GameplayScene::CreateStage()
{
    using namespace SimpleMath;

    // ハードウェア乱数源からシードを生成
    static std::random_device seed_gen;

    // シードを使って乱数エンジンを初期化
    std::mt19937 engine(seed_gen());

    std::vector<Vector3> randomPosition =
    {
        Vector3(0.0f, 52.0f, -50.0f),
        Vector3(90.0f, 42.6f, 35.2f),
        Vector3(-120.0f, 55.6f, -150.2f),
        Vector3(190.0f, 40.6f, -70.2f),
    };
    std::shuffle(randomPosition.begin(), randomPosition.end(), engine);
    auto& treasurePosition = randomPosition.front();

    // お宝の生成
    m_treasure = std::make_unique<Treasure>();
    m_treasure->SetPosition(treasurePosition);
    m_treasure->Initialize(GetCommonResources(), m_collisionManager.get());

    m_buildingManager = std::make_unique<BuildingManager>();
    m_buildingManager->Initialize();
    m_buildingManager->RequestCreate(m_collisionManager.get(), GetCommonResources());
   // m_buildingManager->Save();
    // ****** ここまで ******************************************************



}


