/*****************************************************************//**
 * @file    StartingGameplayState.h
 * @brief   ゲームプレイシーンの開始状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/01/27
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StartingGameplayState.h"

#include <string>

// ライブラリ
#include "Library/MyLib/EasingKit/EasingKit.h"

// ファクトリー関連

// データベース関連
#include "Game/Common/Database/TextureDatabase.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Framework/TaskManager/TaskManager.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/PauseNavigator/PauseNavigator.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Screen.h"


// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/InputDeviceSpriteResolver/InputDeviceSpriteResolver.h"

// 状態
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PauseGameplayState.h"


// 所有者
#include "Game/Scene/GameplayScene/GameplayScene.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
StartingGameplayState::StartingGameplayState()
{
	// スプライトの作成
	for (auto& sprite : m_sprites)
	{
		sprite = std::make_unique<Sprite>();
		
	}
}



/**
 * @brief デストラクタ
 */
StartingGameplayState::~StartingGameplayState()
{

 }




/**
 * @brief 状態の開始
 */
void StartingGameplayState::OnStartState()
{
	// スプライトの作成
	CreateSprite();
	
	// キャンバスに登録
	for (auto& sprite : m_sprites)
	{
		GetOwner()->GetCanvas()->AddSprite(sprite.get());
	}

	// 補間アニメーションの作成
	CreateTween();

	GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver()->AddKeyboardSprite(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)].get());
	GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver()->AddGamePadSprite(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_GAMEPAD)].get());

	// ナビゲーターの作成
	CreatePauseNavigator();

	// コールバックの紐づけ
	RegisterBindCallbackToInput();

	// イベントスタックの作成
	m_eventStack.AddListener(GAME_START_EVENT_NAME, [this]() { this->OnStartGame(); });
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void StartingGameplayState::OnUpdate(float deltaTime)
 {
	m_eventStack.ApplyEvents();

	// 補間アニメーションの更新処理
	UpdateTween(deltaTime);

	// タスクの更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);


}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StartingGameplayState::OnDraw()
{
	GetOwner()->GetTaskManager()->Render(*(MainCamera::GetInstance()->GetCamera()));

}

/**
 * @brief 状態の終了
 * 
 */
void StartingGameplayState::OnExitState()
{
	// キャンバスから削除
	for (auto& sprite : m_sprites)
	{
		GetOwner()->GetCanvas()->RemoveSprite(sprite.get());
	}

	// スプライト切り替え器から削除
	auto inputDeviceResolver = GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver();
	inputDeviceResolver->RemoveGamePadSprite (m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_GAMEPAD)].get());
	inputDeviceResolver->RemoveKeyboardSprite(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)].get());

	// コールバックの紐づけを解除する
	UnBindCallbackToInput();
}

/**
 * @brief 入力とコールバックの紐づけ
 */
void StartingGameplayState::RegisterBindCallbackToInput()
{	// 戻る入力
	auto pInputSystem = GetOwner()->GetCommonResources()->GetInputSystem();
	pInputSystem->GetInputActionMap(InputActionID::Player::MAP_NAME)->BindInputEvent(InputActionID::Player::STEPPING, this,
		[this](const InputEventData& data) { if(this)OnInputStartGame(data); });
}

/**
 * @brief 入力とコールバックの紐づけを解除する
 */
void StartingGameplayState::UnBindCallbackToInput()
{
	// 入力システムの取得
	auto pInputSystem = GetOwner()->GetCommonResources()->GetInputSystem();

	pInputSystem->GetInputActionMap(InputActionID::Player::MAP_NAME)->UnBindAllInputEvent(InputActionID::Player::STEPPING, this);
}

/**
 * @brief ナビゲーターの作成
 * 
 */
void StartingGameplayState::CreatePauseNavigator()
{
	// ポーズナビゲーターの作成
	m_pauseNavigator = std::make_unique<PauseNavigator>();

	// ポーズナビゲーターの初期化処理
	m_pauseNavigator->Initialize(
		GetOwner()->GetCommonResources()->GetResourceManager(),
		GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver(),
		GetOwner()->GetCanvas(),
		GetOwner()->GetCommonResources()->GetInputSystem());

	m_pauseNavigator->SetInputCallback([this](const InputEventData& data) {OnOpenPause(data); });
}

/**
 * @brief ポーズ画面を開く処理
 *
 * @param[in] data 入力イベントデータ
 */
void StartingGameplayState::OnOpenPause(const InputEventData& data)
{
	if (data.inputOption.pressed)
	{
		// ポーズ状態にする
		GetStateMachine()->ChangeState<PauseGameplayState>();
		GetOwner()->GetCommonResources()->SetCopyScreenRequest(true);

	}
}

/**
 * @brief ゲームの開始入力
 * 
 * @param[in] data　入力イベントデータ
 */
void StartingGameplayState::OnInputStartGame(const InputEventData& data)
{
	if (m_backgroundTween.IsEnd() && data.inputOption.pressed)
	{
		m_eventStack.FireEvent(GAME_START_EVENT_NAME);
	}
}

/**
 * @brief ゲーム開始
 */
void StartingGameplayState::OnStartGame()
{
	GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_START);
	GetStateMachine()->ChangeState<NormalGameplayState>();
}

/**
 * @brief スプライトの作成
 */
void StartingGameplayState::CreateSprite()
{
	auto rM = GetOwner()->GetCommonResources()->GetResourceManager();

	auto screen = Screen::Get();
	float masterScale = screen->GetScreenScale();
	// スプライトの設定
	{
		using namespace TextureDatabase;
		auto filepathGetter = [&](TextureID id) { return rM->CreateTexture(TEXTURE_PATH_MAP.at(id)); };


		auto spriteSetter = [&](Sprite* sprite, const SimpleMath::Vector2& position, float scale, bool isVisible)
			{
				sprite->SetPosition(position);
				sprite->SetScale(scale);
				sprite->SetVisible(isVisible);
			};

		m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]	->Initialize(filepathGetter(TextureID::BACKGROUND_INGAME_STARTING));
		m_sprites[static_cast<UINT>(SpriteID::ITEM)]				->Initialize(filepathGetter(TextureID::UI_ITEM_INGAME_STARTING));
		m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_GAMEPAD)]	->Initialize(filepathGetter(TextureID::UI_GUIDE_GAMEPAD_A));
		m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)]	->Initialize(filepathGetter(TextureID::UI_GUIDE_KEYBOARD_SPACE));

		const SimpleMath::Vector2 parentPosition(screen->GetCenterXF(), screen->GetCenterYF() + 200.0f * masterScale);
		const float parentScale = 0.6f * masterScale;

		// 座標設定
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)].get(),	parentPosition + SimpleMath::Vector2(0.0f, 0.0f), parentScale * 0.5f, true);
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::ITEM)].get(),				parentPosition + SimpleMath::Vector2(0.0f, 0.0f), parentScale * 0.5f, true);
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)].get(),	parentPosition + SimpleMath::Vector2(0.0f, 30.f) * masterScale, parentScale * 0.9f, true);
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_GAMEPAD)].get(),	parentPosition + SimpleMath::Vector2(0.0f, 30.f) * masterScale, parentScale * 0.9f, false);

	}
}

/**
 * @brief 補間アニメーションの作成
 */
void StartingGameplayState::CreateTween()
{
	auto screen = Screen::Get();
	float masterScale = screen->GetScreenScale();

	auto backgroundPosition = m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]->GetPosition();

	// 背景の動き
	m_backgroundTween.Initialize(SimpleMath::Vector2(2000.f * masterScale, backgroundPosition.y), m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]->GetPosition(), 1.0f);
	m_backgroundTween.SetEase(MyLib::EaseOutCubic);
	m_backgroundTween.SetDelay(1.f);

	// 装飾の動き
	m_itemAlphaTween.Initialize(0.0f, 1.0f, 0.3f);
	m_itemAlphaTween.SetEase(MyLib::EaseInSine);
	m_itemAlphaTween.SetDelay(1.8f);

	// ガイドの動き
	m_guideAlphaTween.Initialize(0.0f, 1.0f, 2.f);
	m_guideAlphaTween.SetEase([](float value) {return 0.5f - 0.5f * std::cosf(2.0f * 3.141592f * value); });
	m_guideAlphaTween.SetDelay(2.3f);
	m_guideAlphaTween.SetLoop(true);
}

/**
 * @brief 補間アニメーションの更新処理
 * 
 * @param[in] deltaTime　前フレームからの経過時間
 */
void StartingGameplayState::UpdateTween(float deltaTime)
{
	// 補間アニメーションの更新処理
	m_backgroundTween	.Update(deltaTime);
	m_itemAlphaTween	.Update(deltaTime);
	m_guideAlphaTween	.Update(deltaTime);

	// スプライトの更新処理
	m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]	->SetPosition(m_backgroundTween.GetValue());
	m_sprites[static_cast<UINT>(SpriteID::ITEM)]				->SetOpacity(m_itemAlphaTween.GetValue());
	m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)]	->SetOpacity(m_guideAlphaTween.GetValue());
	m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_GAMEPAD)]	->SetOpacity(m_guideAlphaTween.GetValue());
}


