/*****************************************************************//**
 * @file   PauseGameplayState.h
 * @brief  ゲームプレイシーンのポーズ状態に関するソースファイル
 *
 * @author 松下大暉
 * @date   2025/09/15
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PauseGameplayState.h"

// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"

// データベース関連
#include "Game/Common/Database/SoundDatabase.h"
#include "Game/Common/Database/TextureDatabase.h"

// シーン関連
#include "Game/Scene/GameplayScene/GameplayScene.h"
#include "Game/Scene/TitleScene/TitleScene.h"
#include "Game/Scene/Loading/LoadingScreen.h"

// シーン状態関連
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/SoundManager/SoundManager.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/StageManager/StageManager.h"

// グラフィック関連
#include "Game/Common/Screen.h"
#include "Game/Common/TransitionMask/TransitionMask.h"
#include "Game/Common/GameEffect/GameEffectController.h"
#include "Game/Common/Camera/MainCamera/MainCamera.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

// UIメニュー関連
#include "Game/UI/TutorialWindow/TutorialWindow.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PauseGameplayState::PauseGameplayState()
	: m_isDisplayingTutorialWindow{ false }
	, m_isPrevConnectedGamepad{ false }
{

}



/**
 * @brief デストラクタ
 */
PauseGameplayState::~PauseGameplayState()
{

}

/**
 * @brief 開始処理
 * */
void PauseGameplayState::OnStartState()
{
	using namespace SimpleMath;

	auto resourceManager	= GetOwner()->GetCommonResources()->GetResourceManager();
	auto context			= GetOwner()->GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto screen = Screen::Get();
	// ***** キャンバスの作成 *********************************************
	m_canvas = std::make_unique<Canvas>(context, GetOwner()->GetCommonResources()->GetCommonStates());
	m_canvas->SetOt(0);  // 一番手前にする

	// **** スプライトの作成 **********************************************
	m_backgroundAlphaSprite = std::make_unique<Sprite>();
	m_pauseFontSprite = std::make_unique<Sprite>();
	m_operatingFontSprite = std::make_unique<Sprite>();
	m_operatingSprite = std::make_unique<Sprite>();
	m_manualSprite = std::make_unique<Sprite>();
	m_backInGameplayingSprite = std::make_unique<Sprite>();


	// キャンバスへ登録
	m_canvas->AddSprite(m_backInGameplayingSprite.get());
	m_canvas->AddSprite(m_backgroundAlphaSprite.get());
	m_canvas->AddSprite(m_pauseFontSprite.get());
	m_canvas->AddSprite(m_operatingFontSprite.get());
	m_canvas->AddSprite(m_operatingSprite.get());
	m_canvas->AddSprite(m_manualSprite.get());

	using namespace TextureDatabase;
	// スプライトのテクスチャ設定
	m_backgroundAlphaSprite	->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_PAUSE_ALPHA_MASK)));
	m_pauseFontSprite		->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_PAUSE_FONT_PAUSE)));
	m_operatingFontSprite	->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_PAUSE_FONT_OPERATING)));
	m_operatingSprite		->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_GAME_OPERATING_KEYBOARD)));
	m_manualSprite			->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_KEYBOARD)));


	// スプライトの座標設定
	m_backgroundAlphaSprite	->SetPosition(Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_pauseFontSprite		->SetPosition(Vector2(screen->GetLeftF() + FONT_PAUSE_POS_X_OFFSET * screen->GetScreenScale(),			screen->GetBottomF() - FONT_PAUSE_POS_Y_OFFSET * screen->GetScreenScale()));
	m_operatingFontSprite	->SetPosition(Vector2(screen->GetRightF() - FONT_OPERATING_POS_X_OFFSET * screen->GetScreenScale(),		screen->GetTopF() + FONT_OPERATING_POS_Y_OFFSET * screen->GetScreenScale()));
	m_operatingSprite		->SetPosition(Vector2(screen->GetRightF() - OPERATING_UI_POS_X_OFFSET * screen->GetScreenScale(),		screen->GetCenterYF() + OPERATING_UI_POS_Y_OFFSET * screen->GetScreenScale()));
	m_manualSprite			->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + MANUAL_UI_POS_X_OFFSET * screen->GetScreenScale(),screen->GetBottomF() - MANUAL_UI_POS_Y_OFFSET * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_backgroundAlphaSprite	->SetScale(ALPHA_SPRITE_SCALE * screen->GetScreenScale());
	m_pauseFontSprite		->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale());
	m_operatingFontSprite	->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale());
	m_operatingSprite		->SetScale(OPERATING_SPRITE_SCALE_PC * screen->GetScreenScale());
	m_manualSprite			->SetScale(MANUAL_SPRITE_SCALE * screen->GetScreenScale());

	// **** 2D用ラインの作成 **********************************************
	// 線の設定
	m_lines.resize(2);
	for (int i = 0; i < m_lines.size(); i++)
	{
		m_lines[i] = std::make_unique<Line2D>();
	}

	m_lines[0]->Initialize(GetOwner()->GetCommonResources()->GetDeviceResources());
	m_lines[1]->Initialize(GetOwner()->GetCommonResources()->GetDeviceResources());

	m_lines[0]->SetAngle(XMConvertToRadians(LINE0_ANGLE_DEGREE));
	m_lines[1]->SetAngle(XMConvertToRadians(LINE1_ANGLE_DEGREE));

	m_lines[0]->SetPosition(Vector2(screen->GetCenterXF() - LINE0_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF()));
	m_lines[1]->SetPosition(Vector2(screen->GetCenterXF() + LINE1_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF()));

	m_lines[0]->SetLength(LINE_LENGTH * screen->GetScreenScale());
	m_lines[1]->SetLength(LINE_LENGTH * screen->GetScreenScale());

	m_lines[0]->SetThickness(LINE_THICKNESS * screen->GetScreenScale());
	m_lines[1]->SetThickness(LINE_THICKNESS * screen->GetScreenScale());

	// ポーズメニューの作成 **********************************************
	m_pauseMenu = std::make_unique<PauseMenu>();
	m_pauseMenu->Initialize(m_canvas.get(), GetOwner()->GetCommonResources(), [&](PauseMenu::MenuItem pushItem) {
		OnPushMenuItem(pushItem); });

	// チュートリアルウィンドウの作成 **********************************************
	m_tutorialWindow = std::make_unique<TutorialWindow>();
	m_tutorialWindow->Initialize(resourceManager, [this]() {OnCloseTutorialWindow(); });

	// 入力の作成 **********************************************
	m_systemInput = InputBindingFactory::SystemInputFactory().Create(DefaultSpawnDesc());

	m_isPrevConnectedGamepad = false;


	m_backInGameplayingSprite->Initialize(GetOwner()->GetCommonResources()->GetCopyScreenTexture()->GetShaderResourceView());
	m_backInGameplayingSprite->SetPosition(Vector2(screen->GetCenterXF(), screen->GetCenterYF()));

	// **** タスク管理へ登録 ****
	GetOwner()->GetTaskManager()->AddTask(m_canvas.get());

	// ステージ管理の更新を停止する
	GetOwner()->GetStageManager()->StopUpdating();

	// 効果音
	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_PAUSE_OPEN), false);
}

/**
 * @brief 更新処理
 * * @param[in] deltaTime
 */
void PauseGameplayState::OnUpdate(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// タスク管理の更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);

	// 入力の更新処理
	m_systemInput->Update(
		GetOwner()->GetCommonResources()->GetKeyboardTracker(),
		GetOwner()->GetCommonResources()->GetMouseTracker(),
		GetOwner()->GetCommonResources()->GetGamePadTracker());


	if (m_isDisplayingTutorialWindow == false)
	{
		// ポーズメニューの更新処理
		m_pauseMenu->Update(deltaTime);
		if (m_systemInput->IsInput(InputActionType::SystemActionID::PAUSE, InputSystem<InputActionType::SystemActionID>::InputOption::PRESSED))
		{
			// ゲームに戻る
			ContinueGame();
		}
	}
	else
	{
		// チュートリアルウィンドウの更新処理
		m_tutorialWindow->Update(
			deltaTime,
			GetOwner()->GetCommonResources()->GetKeyboardTracker(),
			GetOwner()->GetCommonResources()->GetMouseTracker(),
			GetOwner()->GetCommonResources()->GetGamePadTracker());
	}

	// ガイドUI変更を試みる
	TryChangeCurrentGuideUI();



}


/**
 * @brief 描画処理
 * */
void PauseGameplayState::OnDraw()
{
	auto states = GetOwner()->GetCommonResources()->GetCommonStates();
	auto screen = Screen::Get();

	// 現在のカメラの取得
	const Camera* pCurrentCamera = MainCamera::GetInstance()->GetCamera();
	// エフェクトにカメラを設定する
	GameEffectController::GetInstance()->SetCamera(pCurrentCamera);
	// タスク管理の描画処理
	GetOwner()->GetTaskManager()->Render(*pCurrentCamera);


	if (m_isDisplayingTutorialWindow == false)
	{
		m_pauseMenu->Draw();

		for (auto& line : m_lines)
		{
			line->Draw(states, screen->GetWidthF(), screen->GetHeightF());
		}
	}


}

void PauseGameplayState::OnExitState()
{
	// キャンバスの削除
	GetOwner()->GetTaskManager()->DeleteTask(m_canvas.get());
}

/**
 * @brief メニューアイテムを押したときに呼ばれる
 * * @param[in] menuItem　押されたメニューアイテム
 */
void PauseGameplayState::OnPushMenuItem(PauseMenu::MenuItem menuItem)
{

	switch (menuItem)
	{
	case PauseMenu::MenuItem::CONTINUE:


		ContinueGame();
		break;
	case PauseMenu::MenuItem::TUTORIAL:
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);

		m_isDisplayingTutorialWindow = true;
		m_canvas->AddSprite(m_tutorialWindow.get());
		break;
	case PauseMenu::MenuItem::SETTING:
		break;
	case PauseMenu::MenuItem::RETURN_TITLE:
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);
		if (GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd())
		{
			// シーンを切り替える
			GetOwner()->GetCommonResources()->GetTransitionMask()->Close([&]()
				{
					GetOwner()->ChangeScene<TitleScene, LoadingScreen>();
					GetOwner()->OnEndScene();

				});
		}
		

		break;
	default:
		break;
	}
}

/**
 * @brief ゲームに戻る
 * */
void PauseGameplayState::ContinueGame()
{
	// SEの再生
	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CANCEL), false);
	GetStateMachine()->ChangeState<NormalGameplayState>();
}

/**
 * @brief チュートリアルウィンドウを閉じる際の処理
 */
void PauseGameplayState::OnCloseTutorialWindow()
{
	m_isDisplayingTutorialWindow = false;
	m_canvas->RemoveSprite(m_tutorialWindow.get());
}


/**
 * @brief 現在のガイドガイドUIの変更を試みる
 *
 * 簡易実装用の後にクラス化する
 *
 * @returns true 成功
 * @returns false 失敗
 */
bool PauseGameplayState::TryChangeCurrentGuideUI()
{
	using namespace TextureDatabase;

	bool requestChange = false;
	bool changePC = true;;

	// ゲームパッドの接続されたらパッドガイドにに切り替える
	if (!m_isPrevConnectedGamepad && GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = false;
		requestChange = true;
		m_isPrevConnectedGamepad = true;
	}

	// ゲームパッドが接続されなくなったらPCガイドに切り替える
	if (m_isPrevConnectedGamepad && !GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = true;
		requestChange = true;
		m_isPrevConnectedGamepad = false;

	}

	if (requestChange)
	{
		std::string manualFilePath;
		std::string operatingFilePath; // std::string から const char* に型を変更

		if (changePC)
		{
			manualFilePath		= TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_KEYBOARD);
			operatingFilePath	= TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_GAME_OPERATING_KEYBOARD);

		}

		else
		{
			manualFilePath		= TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_GAMEPAD);
			operatingFilePath	= TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_GAME_OPERATING_GAMEPAD);

		}

		// ガイドUI
		// 拡大率と座標を保持する
		float manualScale = m_manualSprite->GetScale();
		SimpleMath::Vector2 manualPosition = m_manualSprite->GetPosition();
		m_manualSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(manualFilePath));
		m_manualSprite->SetScale(manualScale);
		m_manualSprite->SetPosition(manualPosition);


		// 操作説明
		float operatingScale = 0.0f;
		auto screen = Screen::Get();

		// PCとゲームパッドだとサイズが異なるためここで修正
		if (changePC)
		{
			operatingScale = OPERATING_SPRITE_SCALE_PC * screen->GetScreenScale();
		}
		else
		{
			operatingScale = OPERATING_SPRITE_SCALE_GAMEPAD * screen->GetScreenScale();
		}
		SimpleMath::Vector2 operatingPosition = m_operatingSprite->GetPosition();
		m_operatingSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(operatingFilePath));
		m_operatingSprite->SetScale(operatingScale);
		m_operatingSprite->SetPosition(operatingPosition);

		return true;
	}

	return false;
}
