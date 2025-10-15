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

    // BGMを鳴らす
    SoundManager::GetInstance()->RemoveAll();
    SoundManager::GetInstance()->Play(SoundPaths::BGM_INGAME, true);

    DX::DeviceResources*    pDeviceResources    = GetCommonResources()->GetDeviceResources();
    CommonStates*           pStates             = GetCommonResources()->GetCommonStates();
    ID3D11DeviceContext*    context             = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();

    // 衝突管理の生成
    m_collisionManager  =   std::make_unique<CollisionManager>();
    // グリッド床の生成
    m_gridFloor         =   std::make_unique<Imase::GridFloor>(pDeviceResources->GetD3DDevice(), pDeviceResources->GetD3DDeviceContext(), pStates);

    // キャンバスの設定
    m_canvas = std::make_unique<Canvas>();
    // スプライトの生成
    m_scopeSprite = std::make_unique<Sprite>();
 
    // キャンバスの初期化処理
    m_canvas->Initialize(context);

    // スプライトの初期化処理
    m_scopeSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("scope.dds"));
    m_scopeSprite->SetPosition(DirectX::SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF() - 140.f *  Screen::Get()->GetScreenScale()));
    m_scopeSprite->SetScale(0.09f * Screen::Get()->GetScreenScale());
    m_canvas->AddSprite(m_scopeSprite.get());
    m_gameEffectManager = std::make_unique<GameEffectManager>();
    // 現在使用するエフェクト管理の取得
    GameEffectController::GetInstance()->SetGameEffectManager(m_gameEffectManager.get());
    // メインカメラの設定 
    MainCamera::GetInstance()->SetCamera(m_playerCamera.get());
    // **** ゲームオブジェクトの作成 *****    
    // プレイヤーカメラの初期化処理
    m_playerCamera->Initialize(GetCommonResources(), m_collisionManager.get());
    
    // 床の生成
    m_floor = std::make_unique<Floor>();
    // 床の初期化
    m_floor->Initialize(SimpleMath::Vector3(0.0f, 0.0f, 0.0f), GetCommonResources(), m_collisionManager.get());

    // 壁の生成
    // プレイヤーの生成
    m_player = std::make_unique<Player>();
    // プレイヤーの初期化
    m_player->Initialize(GetCommonResources(), m_collisionManager.get());

    // プレイヤー影の作成
    m_playerShadow = std::make_unique<CircularShadow>();
    m_playerShadow->Initialize(GetCommonResources()->GetDeviceResources(), 0.01f);
   
    // プレイヤーコントローラの作成
    m_playerController = std::make_unique<PlayerController>(m_player.get(), m_playerCamera.get());
    // カメラにプレイヤーを設定する
    m_playerCamera->SetPlayer(m_player.get());

    // 敵管理の作成
    m_enemyManager = std::make_unique<EnemyManager>();
    m_enemyManager->Initialize();

    // 出現管理の作成
    m_spawnManager = std::make_unique<SpawnManager>();
    m_spawnManager->Initialize(m_enemyManager.get(), &m_escapeHelicopter, GetCommonResources(), m_collisionManager.get());

    CreateStage();
  
    // モデルの読み込み
    m_skySphere = GetCommonResources()->GetResourceManager()->CreateModel("skyDome.sdkmesh");

    // ステートマシーンの作成
    m_stateMachine = std::make_unique <StateMachine<GameplayScene>>(this);
    // 最初の状態
    m_stateMachine->ChangeState<NormalGameplayState>();

    // 経過時間のリセット
    m_gamePlayingTimeCounter.Reset();

    ResultData::GetInstance()->Reset();



}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime フレーム間の経過時間
 *
 * @return なし
 */
void GameplayScene::Update(float elapsedTime)
{
    // 状態の更新処理
    m_stateMachine->Update(elapsedTime);

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
 * @param[in] elapsedTime 経過時間
 */
void GameplayScene::UpdateInGameObjects(float elapsedTime)
{
    auto kb = Keyboard::Get().GetState();

    // ゲームプレイ時間の加算
    m_gamePlayingTimeCounter.UpperTime(elapsedTime);

    // ゲームエフェクト管理の更新処理
    m_gameEffectManager->Update(elapsedTime);
  
    // 出現管理の更新処理
    m_spawnManager->Update(elapsedTime);

    // デバックカメラの更新
    m_debugCamera->Update();

    auto mouse = Mouse::Get().GetState();

    m_playerCamera->Update(elapsedTime);

    // 衝突管理の更新処理
    m_collisionManager->Update();

    // 壁の更新処理
    //m_wall->Update(elapsedTime);

    for (auto& wall : m_walls)
    {
        wall->Update(elapsedTime);
    }

    // プレイヤーコントローラの更新処理
    m_playerController->Update(elapsedTime, GetCommonResources()->GetKeyboardTracker(), GetCommonResources()->GetMouseTracker());
    // プレイヤーの更新処理
    m_player->Update(elapsedTime, *m_playerCamera, m_proj);

    for (auto& stageObj : m_stageObject)
    {
        stageObj->Update(elapsedTime);
    }

    // お宝の更新処理
    m_treasure->Update(elapsedTime);

    // 敵の更新処理
    m_enemyManager->Update(elapsedTime);

    // 脱出用ヘリコプターの更新処理
    for (auto& helicopter : m_escapeHelicopter)
    {
        helicopter->Update(elapsedTime);
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

    // 壁の描画処理
    //m_wall->Draw(&view, &m_proj);

    for (auto& wall : m_walls)
    {
        wall->Draw(&view, &m_proj);
    }

    //m_floor->Draw(&view, &m_proj);

    // プレイヤーの描画処理
    m_player->Draw(view, m_proj);


    for (auto& stageObj : m_stageObject)
    {
        stageObj->Draw(view, m_proj);
    }

    // 建物の描画処理
    for (auto& building : m_buildings)
    {
        building->Draw(view, m_proj);
    }

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
            ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetElapsedTime(), false);
            ChangeScene<ResultScene, LoadingScreen>();
            });
        break;
    case GameFlowEventID::ESCAPE_SUCCESS:
        m_eventStack.emplace_back([&]() {
            OnEndScene();
            ResultData::GetInstance()->SetResultData(m_gamePlayingTimeCounter.GetElapsedTime(),  true);
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

    // ****　要修正 !!! :　仮でステージを生成する殴り書きコード *****************************************
    // ****                マネージャークラス、生成クラスを実装予定

    struct BuildingTrans
    {
    public:
        Vector3 pos;
        Vector3 scale;

        BuildingTrans(Vector3 pos, Vector3 scale)
            : pos{ pos }
            , scale{ scale }
        {
        }
    };
    std::vector<BuildingTrans> buildingTrans
    {
        {Vector3(0.0f, 0.0f, 0.0f), Vector3(1.18f, 2.51f, 1.25f)},
        {Vector3(0.0f, 0.0f, -50.0f), Vector3(2.0f, 2.5f, 2.5f)},
        {Vector3(-25.2f, 0.0f, -12.5f), Vector3(1.34f, 1.98f, 1.42f)},
        {Vector3(15.9f, 0.0f, 31.6f), Vector3(1.11f, 2.82f, 1.05f)},
        {Vector3(48.7f, 0.0f, 22.3f), Vector3(1.04f, 1.67f, 1.33f)},
        {Vector3(-35.1f, 0.0f, -44.8f), Vector3(1.22f, 2.30f, 1.48f)},
        {Vector3(-61.4f, 0.0f, 8.9f), Vector3(1.47f, 1.77f, 1.15f)},
        {Vector3(-55.6f, 0.0f, 47.2f), Vector3(1.30f, 2.94f, 1.09f)},
        {Vector3(33.0f, 0.0f, -58.4f), Vector3(1.17f, 1.55f, 1.29f)},
        {Vector3(9.2f, 0.0f, 72.5f), Vector3(1.41f, 2.12f, 1.01f)},
        {Vector3(66.8f, 0.0f, -39.1f), Vector3(1.06f, 1.88f, 1.38f)},
        {Vector3(42.3f, 0.0f, -85.0f), Vector3(1.25f, 2.76f, 1.45f)},
        {Vector3(-78.1f, 0.0f, -25.6f), Vector3(1.36f, 2.45f, 1.03f)},
        {Vector3(71.5f, 0.0f, 55.7f), Vector3(1.02f, 1.91f, 1.27f)},
        {Vector3(-5.8f, 0.0f, -98.3f), Vector3(1.19f, 2.68f, 1.49f)},
        {Vector3(-89.0f, 0.0f, 33.4f), Vector3(1.44f, 2.05f, 1.12f)},
        {Vector3(95.7f, 0.0f, 9.8f), Vector3(1.07f, 2.97f, 1.30f)},
        {Vector3(-76.2f, 0.0f, -82.1f), Vector3(1.32f, 1.63f, 1.44f)},
        {Vector3(57.4f, 0.0f, 105.6f), Vector3(1.01f, 2.22f, 1.06f)},
        {Vector3(-110.3f, 0.0f, -1.5f), Vector3(1.20f, 2.89f, 1.36f)},
        {Vector3(-93.6f, 0.0f, -68.2f), Vector3(1.46f, 1.74f, 1.18f)},
        {Vector3(121.0f, 0.0f, 30.7f), Vector3(1.09f, 2.58f, 1.40f)},
        {Vector3(88.5f, 0.0f, 92.4f), Vector3(1.28f, 2.15f, 1.04f)},
        {Vector3(-3.4f, 0.0f, 124.1f), Vector3(1.05f, 1.83f, 1.26f)},
        {Vector3(-132.1f, 0.0f, 45.9f), Vector3(1.24f, 2.62f, 1.41f)},
        {Vector3(81.8f, 0.0f, -115.0f), Vector3(1.39f, 2.36f, 1.08f)},
        {Vector3(139.3f, 0.0f, -20.6f), Vector3(1.03f, 1.94f, 1.31f)},
        {Vector3(-22.8f, 0.0f, -141.7f), Vector3(1.21f, 2.74f, 1.46f)},
        {Vector3(-118.9f, 0.0f, 110.0f), Vector3(1.45f, 2.09f, 1.13f)},
        {Vector3(-150.0f, 0.0f, 71.3f), Vector3(1.04f, 2.91f, 1.32f)},
        {Vector3(115.4f, 0.0f, -100.1f), Vector3(1.27f, 1.59f, 1.43f)},
        {Vector3(165.6f, 0.0f, 5.2f), Vector3(1.01f, 2.25f, 1.07f)},
        {Vector3(75.1f, 0.0f, 135.8f), Vector3(1.23f, 2.85f, 1.37f)},
        {Vector3(-148.9f, 0.0f, -110.1f), Vector3(1.48f, 1.71f, 1.19f)},
        {Vector3(-158.2f, 0.0f, -40.9f), Vector3(1.10f, 2.54f, 1.45f)},
        {Vector3(178.4f, 0.0f, 56.3f), Vector3(1.29f, 2.01f, 1.02f)},
        {Vector3(-55.3f, 0.0f, 152.0f), Vector3(1.05f, 1.78f, 1.28f)},
        {Vector3(120.1f, 0.0f, -148.5f), Vector3(1.26f, 2.65f, 1.47f)},
        {Vector3(-170.5f, 0.0f, 118.7f), Vector3(1.42f, 2.42f, 1.10f)},
        {Vector3(190.0f, 0.0f, -70.9f), Vector3(1.06f, 1.97f, 1.35f)},
        {Vector3(150.7f, 0.0f, 145.0f), Vector3(1.24f, 2.72f, 1.49f)},
        {Vector3(-35.8f, 0.0f, -179.6f), Vector3(1.49f, 2.03f, 1.14f)},
        {Vector3(-195.1f, 0.0f, -10.4f), Vector3(1.08f, 2.88f, 1.34f)},
        {Vector3(2.3f, 0.0f, 191.2f), Vector3(1.35f, 1.56f, 1.46f)},
        {Vector3(-165.7f, 0.0f, -135.8f), Vector3(1.12f, 2.21f, 1.09f)},
        {Vector3(205.2f, 0.0f, 100.3f), Vector3(1.33f, 2.84f, 1.39f)},
        {Vector3(-28.6f, 0.0f, -210.5f), Vector3(1.17f, 1.75f, 1.11f)},
        {Vector3(-215.3f, 0.0f, -75.1f), Vector3(1.04f, 2.56f, 1.31f)},
        {Vector3(190.4f, 0.0f, -170.2f), Vector3(1.28f, 2.18f, 1.44f)},
        {Vector3(230.5f, 0.0f, 5.9f), Vector3(1.47f, 2.99f, 1.05f)},
        {Vector3(-88.8f, 0.0f, 215.1f), Vector3(1.03f, 1.62f, 1.25f)},
        {Vector3(-210.7f, 0.0f, 150.3f), Vector3(1.22f, 2.33f, 1.48f)},
        {Vector3(245.6f, 0.0f, -102.4f), Vector3(1.38f, 1.80f, 1.16f)},
        {Vector3(240.8f, 0.0f, 80.8f), Vector3(1.09f, 2.52f, 1.40f)},
        {Vector3(-115.1f, 0.0f, -230.2f), Vector3(1.31f, 2.08f, 1.03f)},
        {Vector3(40.2f, 0.0f, -245.5f), Vector3(1.15f, 1.93f, 1.34f)},
        {Vector3(-220.3f, 0.0f, -160.6f), Vector3(1.27f, 2.79f, 1.45f)},
        {Vector3(225.4f, 0.0f, 140.7f), Vector3(1.43f, 2.40f, 1.12f)},
        {Vector3(-250.5f, 0.0f, 10.3f), Vector3(1.06f, 1.90f, 1.36f)},
        {Vector3(-155.6f, 0.0f, 250.4f), Vector3(1.25f, 2.61f, 1.49f)},
        {Vector3(210.7f, 0.0f, 220.5f), Vector3(1.41f, 2.35f, 1.07f)},
        {Vector3(-10.1f, 0.0f, -240.2f), Vector3(1.18f, 2.51f, 1.25f)},
        {Vector3(28.2f, 0.0f, -112.5f), Vector3(1.34f, 1.98f, 1.42f)},
        {Vector3(-15.9f, 0.0f, 31.6f), Vector3(1.11f, 2.82f, 1.05f)},
        {Vector3(-48.7f, 0.0f, 22.3f), Vector3(1.04f, 1.67f, 1.33f)},
        {Vector3(35.1f, 0.0f, -44.8f), Vector3(1.22f, 2.30f, 1.48f)},
        {Vector3(61.4f, 0.0f, 8.9f), Vector3(1.47f, 1.77f, 1.15f)},
        {Vector3(55.6f, 0.0f, 47.2f), Vector3(1.30f, 2.94f, 1.09f)},
        {Vector3(-33.0f, 0.0f, -58.4f), Vector3(1.17f, 1.55f, 1.29f)},
        {Vector3(-9.2f, 0.0f, 72.5f), Vector3(1.41f, 2.12f, 1.01f)},
        {Vector3(-66.8f, 0.0f, -39.1f), Vector3(1.06f, 1.88f, 1.38f)},
        {Vector3(-42.3f, 0.0f, -85.0f), Vector3(1.25f, 2.76f, 1.45f)},
        {Vector3(78.1f, 0.0f, -25.6f), Vector3(1.36f, 2.45f, 1.03f)},
        {Vector3(-71.5f, 0.0f, 55.7f), Vector3(1.02f, 1.91f, 1.27f)},
        {Vector3(5.8f, 0.0f, -98.3f), Vector3(1.19f, 2.68f, 1.49f)},
        {Vector3(-95.7f, 0.0f, 9.8f), Vector3(1.07f, 2.97f, 1.30f)},
        {Vector3(76.2f, 0.0f, -82.1f), Vector3(1.32f, 1.63f, 1.44f)},
        {Vector3(-57.4f, 0.0f, 105.6f), Vector3(1.01f, 2.22f, 1.06f)},
        {Vector3(110.3f, 0.0f, -1.5f), Vector3(1.20f, 2.89f, 1.36f)},
        {Vector3(93.6f, 0.0f, -68.2f), Vector3(1.46f, 1.74f, 1.18f)},
        {Vector3(-121.0f, 0.0f, 30.7f), Vector3(1.09f, 2.58f, 1.40f)},
        {Vector3(-88.5f, 0.0f, 92.4f), Vector3(1.28f, 2.15f, 1.04f)},
        {Vector3(3.4f, 0.0f, 124.1f), Vector3(1.05f, 1.83f, 1.26f)},
        {Vector3(132.1f, 0.0f, 45.9f), Vector3(1.24f, 2.62f, 1.41f)},
        {Vector3(-81.8f, 0.0f, -115.0f), Vector3(1.39f, 2.36f, 1.08f)},
        {Vector3(-139.3f, 0.0f, -20.6f), Vector3(1.03f, 1.94f, 1.31f)},
        {Vector3(22.8f, 0.0f, -141.7f), Vector3(1.21f, 2.74f, 1.46f)},
        {Vector3(118.9f, 0.0f, 110.0f), Vector3(1.45f, 2.09f, 1.13f)},
        {Vector3(150.0f, 0.0f, 71.3f), Vector3(1.04f, 2.91f, 1.32f)},
        {Vector3(-115.4f, 0.0f, -100.1f), Vector3(1.27f, 1.59f, 1.43f)},
        {Vector3(-165.6f, 0.0f, 5.2f), Vector3(1.01f, 2.25f, 1.07f)},
        {Vector3(-75.1f, 0.0f, 135.8f), Vector3(1.23f, 2.85f, 1.37f)},
        {Vector3(148.9f, 0.0f, -110.1f), Vector3(1.48f, 1.71f, 1.19f)},
        {Vector3(158.2f, 0.0f, -40.9f), Vector3(1.10f, 2.54f, 1.45f)},
        {Vector3(-178.4f, 0.0f, 56.3f), Vector3(1.29f, 2.01f, 1.02f)},
        {Vector3(55.3f, 0.0f, 152.0f), Vector3(1.05f, 1.78f, 1.28f)},
        {Vector3(-120.1f, 0.0f, -148.5f), Vector3(1.26f, 2.65f, 1.47f)},
        {Vector3(170.5f, 0.0f, 118.7f), Vector3(1.42f, 2.42f, 1.10f)},
        {Vector3(-190.0f, 0.0f, -70.9f), Vector3(1.06f, 1.97f, 1.35f)},
        {Vector3(-150.7f, 0.0f, 145.0f), Vector3(1.24f, 2.72f, 1.49f)},
        {Vector3(35.8f, 0.0f, -179.6f), Vector3(1.49f, 2.03f, 1.14f)},
        {Vector3(195.1f, 0.0f, -10.4f), Vector3(1.08f, 2.88f, 1.34f)},
        {Vector3(-2.3f, 0.0f, 191.2f), Vector3(1.35f, 1.56f, 1.46f)},
        {Vector3(165.7f, 0.0f, -135.8f), Vector3(1.12f, 2.21f, 1.09f)},
        {Vector3(-205.2f, 0.0f, 100.3f), Vector3(1.33f, 2.84f, 1.39f)},
        {Vector3(28.6f, 0.0f, -210.5f), Vector3(1.17f, 1.75f, 1.11f)},
        {Vector3(215.3f, 0.0f, -75.1f), Vector3(1.04f, 2.56f, 1.31f)},
        {Vector3(-190.4f, 0.0f, -170.2f), Vector3(1.28f, 2.18f, 1.44f)},
        {Vector3(-230.5f, 0.0f, 5.9f), Vector3(1.47f, 2.99f, 1.05f)},
        {Vector3(88.8f, 0.0f, 215.1f), Vector3(1.03f, 1.62f, 1.25f)},
        {Vector3(210.7f, 0.0f, 150.3f), Vector3(1.22f, 2.33f, 1.48f)},
        {Vector3(-245.6f, 0.0f, -102.4f), Vector3(1.38f, 1.80f, 1.16f)},
        {Vector3(-240.8f, 0.0f, 80.8f), Vector3(1.09f, 2.52f, 1.40f)},
        {Vector3(115.1f, 0.0f, -230.2f), Vector3(1.31f, 2.08f, 1.03f)},
        {Vector3(-40.2f, 0.0f, -245.5f), Vector3(1.15f, 1.93f, 1.34f)},
        {Vector3(220.3f, 0.0f, -160.6f), Vector3(1.27f, 2.79f, 1.45f)},
        {Vector3(-225.4f, 0.0f, 140.7f), Vector3(1.43f, 2.40f, 1.12f)},
        {Vector3(250.5f, 0.0f, 10.3f), Vector3(1.06f, 1.90f, 1.36f)},
        {Vector3(155.6f, 0.0f, 250.4f), Vector3(1.25f, 2.61f, 1.49f)},
        {Vector3(-210.7f, 0.0f, 220.5f), Vector3(1.41f, 2.35f, 1.07f)},
        {Vector3(-10.1f, 0.0f, -240.2f), Vector3(1.18f, 2.51f, 1.25f)},
        {Vector3(28.2f, 0.0f, -112.5f), Vector3(1.34f, 1.98f, 1.42f)},
        {Vector3(-15.9f, 0.0f, 31.6f), Vector3(1.11f, 2.82f, 1.05f)},
        {Vector3(-48.7f, 0.0f, 22.3f), Vector3(1.04f, 1.67f, 1.33f)},
        {Vector3(35.1f, 0.0f, -44.8f), Vector3(1.22f, 2.30f, 1.48f)},
        {Vector3(61.4f, 0.0f, 8.9f), Vector3(1.47f, 1.77f, 1.15f)},
        {Vector3(55.6f, 0.0f, 47.2f), Vector3(1.30f, 2.94f, 1.09f)},
        {Vector3(-33.0f, 0.0f, -58.4f), Vector3(1.17f, 1.55f, 1.29f)},
        {Vector3(-9.2f, 0.0f, 72.5f), Vector3(1.41f, 2.12f, 1.01f)},
        {Vector3(-66.8f, 0.0f, -39.1f), Vector3(1.06f, 1.88f, 1.38f)},
        {Vector3(-42.3f, 0.0f, -85.0f), Vector3(1.25f, 2.76f, 1.45f)},
        {Vector3(78.1f, 0.0f, -25.6f), Vector3(1.36f, 2.45f, 1.03f)},
        {Vector3(-71.5f, 0.0f, 55.7f), Vector3(1.02f, 1.91f, 1.27f)},
        {Vector3(5.8f, 0.0f, -98.3f), Vector3(1.19f, 2.68f, 1.49f)},
        {Vector3(89.0f, 0.0f, 33.4f), Vector3(1.44f, 2.05f, 1.12f)},
        {Vector3(-95.7f, 0.0f, 9.8f), Vector3(1.07f, 2.97f, 1.30f)},
        {Vector3(76.2f, 0.0f, -82.1f), Vector3(1.32f, 1.63f, 1.44f)},
        {Vector3(-57.4f, 0.0f, 105.6f), Vector3(1.01f, 2.22f, 1.06f)},
        {Vector3(110.3f, 0.0f, -1.5f), Vector3(1.20f, 2.89f, 1.36f)},
        {Vector3(93.6f, 0.0f, -68.2f), Vector3(1.46f, 1.74f, 1.18f)},
        {Vector3(-121.0f, 0.0f, 30.7f), Vector3(1.09f, 2.58f, 1.40f)},
        {Vector3(-88.5f, 0.0f, 92.4f), Vector3(1.28f, 2.15f, 1.04f)},
        {Vector3(3.4f, 0.0f, 124.1f), Vector3(1.05f, 1.83f, 1.26f)},
        {Vector3(132.1f, 0.0f, 45.9f), Vector3(1.24f, 2.62f, 1.41f)},
        {Vector3(-81.8f, 0.0f, -115.0f), Vector3(1.39f, 2.36f, 1.08f)},
        {Vector3(-139.3f, 0.0f, -20.6f), Vector3(1.03f, 1.94f, 1.31f)},
        {Vector3(22.8f, 0.0f, -141.7f), Vector3(1.21f, 2.74f, 1.46f)},
        {Vector3(118.9f, 0.0f, 110.0f), Vector3(1.45f, 2.09f, 1.13f)},
        {Vector3(150.0f, 0.0f, 71.3f), Vector3(1.04f, 2.91f, 1.32f)},
        {Vector3(-115.4f, 0.0f, -100.1f), Vector3(1.27f, 1.59f, 1.43f)},
        {Vector3(-165.6f, 0.0f, 5.2f), Vector3(1.01f, 2.25f, 1.07f)},
        {Vector3(-75.1f, 0.0f, 135.8f), Vector3(1.23f, 2.85f, 1.37f)},
        {Vector3(148.9f, 0.0f, -110.1f), Vector3(1.48f, 1.71f, 1.19f)},
        {Vector3(158.2f, 0.0f, -40.9f), Vector3(1.10f, 2.54f, 1.45f)},
        {Vector3(-178.4f, 0.0f, 56.3f), Vector3(1.29f, 2.01f, 1.02f)},
        {Vector3(55.3f, 0.0f, 152.0f), Vector3(1.05f, 1.78f, 1.28f)},
        {Vector3(-120.1f, 0.0f, -148.5f), Vector3(1.26f, 2.65f, 1.47f)},
        {Vector3(170.5f, 0.0f, 118.7f), Vector3(1.42f, 2.42f, 1.10f)},
        {Vector3(-190.0f, 0.0f, -70.9f), Vector3(1.06f, 1.97f, 1.35f)},
        {Vector3(-150.7f, 0.0f, 145.0f), Vector3(1.24f, 2.72f, 1.49f)},
        {Vector3(35.8f, 0.0f, -179.6f), Vector3(1.49f, 2.03f, 1.14f)},
        {Vector3(195.1f, 0.0f, -10.4f), Vector3(1.08f, 2.88f, 1.34f)},
        {Vector3(-2.3f, 0.0f, 191.2f), Vector3(1.35f, 1.56f, 1.46f)},
        {Vector3(165.7f, 0.0f, -135.8f), Vector3(1.12f, 2.21f, 1.09f)},
        {Vector3(-205.2f, 0.0f, 100.3f), Vector3(1.33f, 2.84f, 1.39f)},
        {Vector3(28.6f, 0.0f, -210.5f), Vector3(1.17f, 1.75f, 1.11f)},
        {Vector3(215.3f, 0.0f, -75.1f), Vector3(1.04f, 2.56f, 1.31f)},
        {Vector3(-190.4f, 0.0f, -170.2f), Vector3(1.28f, 2.18f, 1.44f)},
        {Vector3(-230.5f, 0.0f, 5.9f), Vector3(1.47f, 2.99f, 1.05f)},
        {Vector3(88.8f, 0.0f, 215.1f), Vector3(1.03f, 1.62f, 1.25f)},
        {Vector3(210.7f, 0.0f, 150.3f), Vector3(1.22f, 2.33f, 1.48f)},
        {Vector3(-245.6f, 0.0f, -102.4f), Vector3(1.38f, 1.80f, 1.16f)},
        {Vector3(-240.8f, 0.0f, 80.8f), Vector3(1.09f, 2.52f, 1.40f)},
        {Vector3(115.1f, 0.0f, -230.2f), Vector3(1.31f, 2.08f, 1.03f)},
        {Vector3(-40.2f, 0.0f, -245.5f), Vector3(1.15f, 1.93f, 1.34f)},
        {Vector3(220.3f, 0.0f, -160.6f), Vector3(1.27f, 2.79f, 1.45f)},
        {Vector3(-225.4f, 0.0f, 140.7f), Vector3(1.43f, 2.40f, 1.12f)},
        {Vector3(250.5f, 0.0f, 10.3f), Vector3(1.06f, 1.90f, 1.36f)},
        {Vector3(155.6f, 0.0f, 250.4f), Vector3(1.25f, 2.61f, 1.49f)},
        {Vector3(-210.7f, 0.0f, 220.5f), Vector3(1.41f, 2.35f, 1.07f)},
        {Vector3(-10.1f, 0.0f, -240.2f), Vector3(1.18f, 2.51f, 1.25f)},
        {Vector3(28.2f, 0.0f, -112.5f), Vector3(1.34f, 1.98f, 1.42f)},
        {Vector3(-15.9f, 0.0f, 31.6f), Vector3(1.11f, 2.82f, 1.05f)},
        {Vector3(-48.7f, 0.0f, 22.3f), Vector3(1.04f, 1.67f, 1.33f)},
        {Vector3(35.1f, 0.0f, -44.8f), Vector3(1.22f, 2.30f, 1.48f)},
        {Vector3(61.4f, 0.0f, 8.9f), Vector3(1.47f, 1.77f, 1.15f)},
        {Vector3(55.6f, 0.0f, 47.2f), Vector3(1.30f, 2.94f, 1.09f)},
        {Vector3(-33.0f, 0.0f, -58.4f), Vector3(1.17f, 1.55f, 1.29f)},
        {Vector3(-9.2f, 0.0f, 72.5f), Vector3(1.41f, 2.12f, 1.01f)},
        {Vector3(-66.8f, 0.0f, -39.1f), Vector3(1.06f, 1.88f, 1.38f)},
        {Vector3(-42.3f, 0.0f, -85.0f), Vector3(1.25f, 2.76f, 1.45f)},
        {Vector3(78.1f, 0.0f, -25.6f), Vector3(1.36f, 2.45f, 1.03f)},
        {Vector3(-71.5f, 0.0f, 55.7f), Vector3(1.02f, 1.91f, 1.27f)},
        {Vector3(5.8f, 0.0f, -98.3f), Vector3(1.19f, 2.68f, 1.49f)},
        {Vector3(-95.7f, 0.0f, 9.8f), Vector3(1.07f, 2.97f, 1.30f)},
        {Vector3(76.2f, 0.0f, -82.1f), Vector3(1.32f, 1.63f, 1.44f)},
        {Vector3(-57.4f, 0.0f, 105.6f), Vector3(1.01f, 2.22f, 1.06f)},
        {Vector3(110.3f, 0.0f, -1.5f), Vector3(1.20f, 2.89f, 1.36f)},
        {Vector3(93.6f, 0.0f, -68.2f), Vector3(1.46f, 1.74f, 1.18f)},
        {Vector3(-121.0f, 0.0f, 30.7f), Vector3(1.09f, 2.58f, 1.40f)},
        {Vector3(-88.5f, 0.0f, 92.4f), Vector3(1.28f, 2.15f, 1.04f)},
        {Vector3(3.4f, 0.0f, 124.1f), Vector3(1.05f, 1.83f, 1.26f)},
        {Vector3(132.1f, 0.0f, 45.9f), Vector3(1.24f, 2.62f, 1.41f)},
        {Vector3(-81.8f, 0.0f, -115.0f), Vector3(1.39f, 2.36f, 1.08f)},
        {Vector3(-139.3f, 0.0f, -20.6f), Vector3(1.03f, 1.94f, 1.31f)},
        {Vector3(22.8f, 0.0f, -141.7f), Vector3(1.21f, 2.74f, 1.46f)},
        {Vector3(118.9f, 0.0f, 110.0f), Vector3(1.45f, 2.09f, 1.13f)},
        {Vector3(150.0f, 0.0f, 71.3f), Vector3(1.04f, 2.91f, 1.32f)},
        {Vector3(-115.4f, 0.0f, -100.1f), Vector3(1.27f, 1.59f, 1.43f)},
        {Vector3(-165.6f, 0.0f, 5.2f), Vector3(1.01f, 2.25f, 1.07f)},
        {Vector3(-75.1f, 0.0f, 135.8f), Vector3(1.23f, 2.85f, 1.37f)},
        {Vector3(148.9f, 0.0f, -110.1f), Vector3(1.48f, 1.71f, 1.19f)},
        {Vector3(158.2f, 0.0f, -40.9f), Vector3(1.10f, 2.54f, 1.45f)},
        {Vector3(-178.4f, 0.0f, 56.3f), Vector3(1.29f, 2.01f, 1.02f)},
        {Vector3(55.3f, 0.0f, 152.0f), Vector3(1.05f, 1.78f, 1.28f)},
        {Vector3(-120.1f, 0.0f, -148.5f), Vector3(1.26f, 2.65f, 1.47f)},
        {Vector3(170.5f, 0.0f, 118.7f), Vector3(1.42f, 2.42f, 1.10f)},
        {Vector3(-190.0f, 0.0f, -70.9f), Vector3(1.06f, 1.97f, 1.35f)},
        {Vector3(-150.7f, 0.0f, 145.0f), Vector3(1.24f, 2.72f, 1.49f)},
        {Vector3(35.8f, 0.0f, -179.6f), Vector3(1.49f, 2.03f, 1.14f)},
        {Vector3(195.1f, 0.0f, -10.4f), Vector3(1.08f, 2.88f, 1.34f)},
        {Vector3(-2.3f, 0.0f, 191.2f), Vector3(1.35f, 1.56f, 1.46f)},
        {Vector3(165.7f, 0.0f, -135.8f), Vector3(1.12f, 2.21f, 1.09f)},
        {Vector3(-205.2f, 0.0f, 100.3f), Vector3(1.33f, 2.84f, 1.39f)},
        {Vector3(28.6f, 0.0f, -210.5f), Vector3(1.17f, 1.75f, 1.11f)},
        {Vector3(215.3f, 0.0f, -75.1f), Vector3(1.04f, 2.56f, 1.31f)},
        {Vector3(-190.4f, 0.0f, -170.2f), Vector3(1.28f, 2.18f, 1.44f)},
        {Vector3(-230.5f, 0.0f, 5.9f), Vector3(1.47f, 2.99f, 1.05f)},
        {Vector3(88.8f, 0.0f, 215.1f), Vector3(1.03f, 1.62f, 1.25f)},
        {Vector3(210.7f, 0.0f, 150.3f), Vector3(1.22f, 2.33f, 1.48f)},
        {Vector3(-245.6f, 0.0f, -102.4f), Vector3(1.38f, 1.80f, 1.16f)},
        {Vector3(-240.8f, 0.0f, 80.8f), Vector3(1.09f, 2.52f, 1.40f)},
        {Vector3(115.1f, 0.0f, -230.2f), Vector3(1.31f, 2.08f, 1.03f)},
        {Vector3(-40.2f, 0.0f, -245.5f), Vector3(1.15f, 1.93f, 1.34f)},
        {Vector3(220.3f, 0.0f, -160.6f), Vector3(1.27f, 2.79f, 1.45f)},
        {Vector3(-225.4f, 0.0f, 140.7f), Vector3(1.43f, 2.40f, 1.12f)},
        {Vector3(250.5f, 0.0f, 10.3f), Vector3(1.06f, 1.90f, 1.36f)},
        {Vector3(155.6f, 0.0f, 250.4f), Vector3(1.25f, 2.61f, 1.49f)},
        {Vector3(-210.7f, 0.0f, 220.5f), Vector3(1.41f, 2.35f, 1.07f)},
    };

    std::sort(buildingTrans.begin(), buildingTrans.end(), [](BuildingTrans& transA, BuildingTrans& transB)
        {
            return (transA.pos.LengthSquared() < transB.pos.LengthSquared());});

    // 重複した要素をuniqueで末尾に移動
    auto last = std::unique(buildingTrans.begin(), buildingTrans.end(), 
        [](BuildingTrans& transA, BuildingTrans& transB) {
            return transA.pos == transB.pos;
        });

    // 末尾から重複要素を削除
    buildingTrans.erase(last, buildingTrans.end());
    // 建物の作成
    for (auto& trans : buildingTrans)
    {
        auto building = std::make_unique<Building>();

        building->SetPosition(trans.pos);
        building->SetExtends(trans.scale);
        building->Initialize(GetCommonResources(), m_collisionManager.get());

        m_buildings.emplace_back(std::move(building));
    }
    // ****** ここまで ******************************************************



}


