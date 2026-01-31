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
#include "Game/Common/Framework/Input/InputManager/InputManager.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/StageManager/StageManager.h"

// グラフィック関連
#include "Game/Common/Screen.h"
#include "Game/Common/Graphics/TransitionMask/TransitionMask.h"
#include "Game/Common/Graphics/GameEffect/GameEffectController.h"
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/InputDeviceSpriteResolver/InputDeviceSpriteResolver.h"

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
	: m_isPrevConnectedGamepad{ false }
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
	// **** スプライトの作成 ****
	CreateSprites();

	// デバイス毎の表示切替をする
	auto pInputDeviceSpriteResolver = GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver();
	pInputDeviceSpriteResolver->AddKeyboardSprite(m_keyboardOperatingSprite.get());
	pInputDeviceSpriteResolver->AddKeyboardSprite(m_keyboardManualSprite.get());
	pInputDeviceSpriteResolver->AddGamePadSprite(m_gamePadOperatingSprite.get());
	pInputDeviceSpriteResolver->AddGamePadSprite(m_gamePadManualSprite.get());

	// ***** キャンバスの作成 ****
	Canvas* pCanvas = GetOwner()->GetCanvas();
	// キャンバスへ登録
	pCanvas->AddSprite(m_backInGameplayingSprite.get());
	pCanvas->AddSprite(m_backgroundAlphaSprite.get());
	pCanvas->AddSprite(m_pauseFontSprite.get());
	pCanvas->AddSprite(m_operatingFontSprite.get());
	pCanvas->AddSprite(m_keyboardOperatingSprite.get());
	pCanvas->AddSprite(m_gamePadOperatingSprite.get());
	pCanvas->AddSprite(m_keyboardManualSprite.get());
	pCanvas->AddSprite(m_gamePadManualSprite.get());


	// **** 2D用ラインの作成 ****
	Create2DLines();

	// ポーズメニューの作成 ****
	m_pauseMenu = std::make_unique<PauseMenu>();
	m_pauseMenu->Initialize(pCanvas, GetOwner()->GetCommonResources(), [&](PauseMenu::MenuItem pushItem) {
		OnPushMenuItem(pushItem); });

	// チュートリアルウィンドウの作成 ****
	m_tutorialWindow = std::make_unique<TutorialWindow>();
	m_tutorialWindow->Initialize(resourceManager, [this]() {OnCloseTutorialWindow(); });


	// **** タスク管理へ登録 ****
	// ステージ管理の更新を停止する
	GetOwner()->GetStageManager()->StopUpdating();

	// 効果音
	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_PAUSE_OPEN), false);

	// 入力のコールバックを登録
	RegisterBindCallbackToInput();

	// メニューイベントの作成
	CreateMenuItemEvent();

	m_isPrevConnectedGamepad = false;
}

/**
 * @brief 更新処理
 * * @param[in] deltaTime
 */
void PauseGameplayState::OnUpdate(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	m_onPushMenuItemEvent->ApplyEvents();

	// タスク管理の更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);

	
	m_pauseMenu->Update(deltaTime);
	
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



	m_pauseMenu->Draw();

	for (auto& line : m_lines)
	{
		line->Draw(states, screen->GetWidthF(), screen->GetHeightF());
	}
	


}

void PauseGameplayState::OnExitState()
{
	// キャンバスから削除
	Canvas* pCanvas = GetOwner()->GetCanvas();
	pCanvas->RemoveSprite(m_backInGameplayingSprite.get());
	pCanvas->RemoveSprite(m_backgroundAlphaSprite.get());
	pCanvas->RemoveSprite(m_pauseFontSprite.get());
	pCanvas->RemoveSprite(m_operatingFontSprite.get());
	pCanvas->RemoveSprite(m_keyboardOperatingSprite.get());
	pCanvas->RemoveSprite(m_gamePadOperatingSprite.get());
	pCanvas->RemoveSprite(m_keyboardManualSprite.get());
	pCanvas->RemoveSprite(m_gamePadManualSprite.get());
	pCanvas->RemoveSprite(m_tutorialWindow.get());

	m_pauseMenu->Finalize();

	// 入力とコールバックの紐づけを解除する
	UnBindCallbackToInput();

	// デバイス毎の表示切替を削除する
	auto pInputDeviceSpriteResolver = GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver();
	pInputDeviceSpriteResolver->RemoveKeyboardSprite(m_keyboardOperatingSprite.get());
	pInputDeviceSpriteResolver->RemoveKeyboardSprite(m_keyboardManualSprite.get());
	pInputDeviceSpriteResolver->RemoveGamePadSprite(m_gamePadOperatingSprite.get());
	pInputDeviceSpriteResolver->RemoveGamePadSprite(m_gamePadManualSprite.get());

}

/**
 * @brief メニューアイテムを押したときに呼ばれる
 * * @param[in] menuItem　押されたメニューアイテム
 */
void PauseGameplayState::OnPushMenuItem(PauseMenu::MenuItem menuItem)
{
	m_onPushMenuItemEvent->FireEvent(menuItem);
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
	m_tutorialWindow->SetVisible(false);
	m_pauseMenu->SetActive(true);

	GetOwner()->GetCanvas()->RemoveSprite(m_tutorialWindow.get());
}

/**
 * @brief コールバックを登録
 */
void PauseGameplayState::RegisterBindCallbackToInput()
{
	// 入力管理の取得
	auto pInputManager = GetOwner()->GetCommonResources()->GetInputManager();

	// 上入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::UP_MOVE, this,
		[this](const InputEventData& data) { OnInputUp(data); });
	// 下入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::DOWN_MOVE, this,
		[this](const InputEventData& data) { OnInputDown(data); });
	// 左入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::LEFT_MOVE, this,
		[this](const InputEventData& data) { OnInputLeft(data); });
	// 右入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::RIGHT_MOVE, this,
		[this](const InputEventData& data) { OnInputRight(data); });
	// 決定入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::CONFIRM, this,
		[this](const InputEventData& data) { OnInputConfirm(data); });
	// 戻る入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::CANCEL, this,
		[this](const InputEventData& data) { OnInputExit(data); });
}

/**
 * @brief コールバックを解除
 */
void PauseGameplayState::UnBindCallbackToInput()
{
	// 入力管理の取得
	auto pInputManager = GetOwner()->GetCommonResources()->GetInputManager();

	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::UP_MOVE, this);
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::LEFT_MOVE, this);
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::RIGHT_MOVE, this);
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::DOWN_MOVE, this);
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::CONFIRM, this);
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::CANCEL, this);
}


/**
 * @brief 左入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void PauseGameplayState::OnInputLeft(const InputEventData& data)
{
	bool isEndTransition = GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd();

	if (isEndTransition && m_tutorialWindow && m_tutorialWindow->IsVisible())
		m_tutorialWindow->OnMoveDownLeft(data);
}

/**
 * @brief 右入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void PauseGameplayState::OnInputRight(const InputEventData& data)
{
	bool isEndTransition = GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd();

	if (isEndTransition && m_tutorialWindow && m_tutorialWindow->IsVisible())
		m_tutorialWindow->OnMoveUpRight(data);
}

/**
 * @brief 上入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void PauseGameplayState::OnInputUp(const InputEventData& data)
{
	bool isEndTransition = GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd();

	if (isEndTransition && m_pauseMenu && m_pauseMenu->IsActive())
		m_pauseMenu->OnMoveUpSelector(data);
}

/**
 * @brief 下入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void PauseGameplayState::OnInputDown(const InputEventData& data)
{
	bool isEndTransition = GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd();

	if (isEndTransition && m_pauseMenu && m_pauseMenu->IsActive())
		m_pauseMenu->OnMoveDownSelector(data);
}

/**
 * @brief 決定入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void PauseGameplayState::OnInputConfirm(const InputEventData& data)
{
	bool isEndTransition = GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd();

	if (isEndTransition && m_pauseMenu && m_pauseMenu->IsActive())
		m_pauseMenu->OnSelect(data);

	if (isEndTransition && m_tutorialWindow && m_tutorialWindow->IsVisible())
		m_tutorialWindow->OnSelect(data);


}

/**
 * @brief 出る入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void PauseGameplayState::OnInputExit(const InputEventData& data)
{
	bool isEndTransition = GetOwner()->GetCommonResources()->GetTransitionMask()->IsEnd();

	if (isEndTransition && data.inputOption.pressed && m_pauseMenu->IsActive())
		ContinueGame();

	if (isEndTransition && m_tutorialWindow->IsVisible())
		m_tutorialWindow->OnSelect(data);
}

/**
 * @brief メニューイベントの作成
 * 
 */
void PauseGameplayState::CreateMenuItemEvent()
{
	m_onPushMenuItemEvent = std::make_unique<EventSystem<PauseMenu::MenuItem>>();

	// プレイボタンを押したとき
	m_onPushMenuItemEvent->AddListener(PauseMenu::MenuItem::CONTINUE, [&]()
		{
			ContinueGame();

		});

	// チュートリアルボタンをを押したとき
	m_onPushMenuItemEvent->AddListener(PauseMenu::MenuItem::TUTORIAL, [&]()
		{
			// SEの再生
			SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);

			m_tutorialWindow->SetVisible(true);
			m_pauseMenu->SetActive(false);
			GetOwner()->GetCanvas()->AddSprite(m_tutorialWindow.get());
		});
	m_onPushMenuItemEvent->AddListener(PauseMenu::MenuItem::SETTING, [&]() {});

	// 終了ボタンを押したとき
	m_onPushMenuItemEvent->AddListener(PauseMenu::MenuItem::RETURN_TITLE, [&]()
		{
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

		});
}

/**
 * @brief スプライトの作成
 * 
 */
void PauseGameplayState::CreateSprites()
{
	auto resourceManager = GetOwner()->GetCommonResources()->GetResourceManager();

	auto screen = Screen::Get();

	// **** スプライトの作成 **********************************************
	m_backgroundAlphaSprite  = std::make_unique<Sprite>();
	m_pauseFontSprite		 = std::make_unique<Sprite>();
	m_operatingFontSprite	 = std::make_unique<Sprite>();
	m_keyboardOperatingSprite= std::make_unique<Sprite>();
	m_gamePadOperatingSprite = std::make_unique<Sprite>();
	m_keyboardManualSprite	 = std::make_unique<Sprite>();
	m_gamePadManualSprite	 = std::make_unique<Sprite>();
	m_backInGameplayingSprite= std::make_unique<Sprite>();

	using namespace TextureDatabase;
	// スプライトのテクスチャ設定
	m_backgroundAlphaSprite	 ->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_PAUSE_ALPHA_MASK)));
	m_pauseFontSprite		 ->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_PAUSE_FONT_PAUSE)));
	m_operatingFontSprite	 ->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_PAUSE_FONT_OPERATING)));
	m_keyboardOperatingSprite->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_GAME_OPERATING_KEYBOARD)));
	m_gamePadOperatingSprite ->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_GAME_OPERATING_GAMEPAD)));
	m_keyboardManualSprite	 ->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_KEYBOARD)));
	m_gamePadManualSprite	 ->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_GAMEPAD)));


	// スプライトの座標設定
	m_backgroundAlphaSprite	->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_pauseFontSprite		->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + FONT_PAUSE_POS_X_OFFSET * screen->GetScreenScale(), screen->GetBottomF() - FONT_PAUSE_POS_Y_OFFSET * screen->GetScreenScale()));
	m_operatingFontSprite	->SetPosition(SimpleMath::Vector2(screen->GetRightF() - FONT_OPERATING_POS_X_OFFSET * screen->GetScreenScale(), screen->GetTopF() + FONT_OPERATING_POS_Y_OFFSET * screen->GetScreenScale()));
	m_keyboardOperatingSprite	->SetPosition(SimpleMath::Vector2(screen->GetRightF() - OPERATING_UI_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF() + OPERATING_UI_POS_Y_OFFSET * screen->GetScreenScale()));
	m_gamePadOperatingSprite	->SetPosition(SimpleMath::Vector2(screen->GetRightF() - OPERATING_UI_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF() + OPERATING_UI_POS_Y_OFFSET * screen->GetScreenScale()));
	m_keyboardManualSprite	->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + MANUAL_UI_POS_X_OFFSET * screen->GetScreenScale(), screen->GetBottomF() - MANUAL_UI_POS_Y_OFFSET * screen->GetScreenScale()));
	m_gamePadManualSprite	->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + MANUAL_UI_POS_X_OFFSET * screen->GetScreenScale(), screen->GetBottomF() - MANUAL_UI_POS_Y_OFFSET * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_backgroundAlphaSprite	->SetScale(ALPHA_SPRITE_SCALE * screen->GetScreenScale());
	m_pauseFontSprite		->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale());
	m_operatingFontSprite	->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale());
	m_keyboardOperatingSprite		->SetScale(OPERATING_SPRITE_SCALE_PC * screen->GetScreenScale());
	m_gamePadOperatingSprite		->SetScale(OPERATING_SPRITE_SCALE_GAMEPAD * screen->GetScreenScale());
	m_keyboardManualSprite		->SetScale(MANUAL_SPRITE_SCALE * screen->GetScreenScale());
	m_gamePadManualSprite			->SetScale(MANUAL_SPRITE_SCALE * screen->GetScreenScale());

	m_backInGameplayingSprite->Initialize(GetOwner()->GetCommonResources()->GetCopyScreenTexture()->GetShaderResourceView());
	m_backInGameplayingSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
}

/**
 * @brief 2Dラインの作成
 * 
 */
void PauseGameplayState::Create2DLines()
{
	auto screen = Screen::Get();

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

	m_lines[0]->SetPosition(SimpleMath::Vector2(screen->GetCenterXF() - LINE0_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF()));
	m_lines[1]->SetPosition(SimpleMath::Vector2(screen->GetCenterXF() + LINE1_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF()));

	m_lines[0]->SetLength(LINE_LENGTH * screen->GetScreenScale());
	m_lines[1]->SetLength(LINE_LENGTH * screen->GetScreenScale());

	m_lines[0]->SetThickness(LINE_THICKNESS * screen->GetScreenScale());
	m_lines[1]->SetThickness(LINE_THICKNESS * screen->GetScreenScale());
}
