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

// ゲームプレイロジック関連

// グラフィック関連
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Screen.h"


// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

// 状態
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"

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

		// キャンバスに登録
		
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
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)].get()	, parentPosition + SimpleMath::Vector2(0.0f, 0.0f)				, parentScale * 0.5f, true);
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::ITEM)].get()					, parentPosition + SimpleMath::Vector2(0.0f, 0.0f)				, parentScale * 0.5f, true);
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)].get()	, parentPosition + SimpleMath::Vector2(0.0f, 30.f) * masterScale, parentScale * 0.9f, true);
		spriteSetter(m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_GAMEPAD)].get()	, parentPosition + SimpleMath::Vector2(0.0f, 30.f) * masterScale, parentScale * 0.9f, false);

	}

	
	// キャンバスに登録
	for (auto& sprite : m_sprites)
	{
		GetOwner()->GetCanvas()->AddSprite(sprite.get());
	}

	m_startPosition = m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]->GetPosition();

	m_backgroundTween.Initialize(SimpleMath::Vector2(2000.f * masterScale,  m_startPosition.y), m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]->GetPosition(), 1.0f);
	m_backgroundTween.SetEase(MyLib::EaseOutCubic);
	m_backgroundTween.SetDelay(1.f);

	m_itemAlphaTween.Initialize(0.0f, 1.0f, 0.3f);
	m_itemAlphaTween.SetEase(MyLib::EaseInSine);
	m_itemAlphaTween.SetDelay(1.8f);

	m_guideAlphaTween.Initialize(0.0f, 1.0f, 2.f);
	m_guideAlphaTween.SetEase([](float value) {return 0.5f - 0.5f * std::cosf(2.0f * 3.141592f * value); });
	m_guideAlphaTween.SetDelay(2.3f);
	m_guideAlphaTween.SetLoop(true);

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
	m_elapsedTimeCounter.UpperTime(deltaTime);

	m_backgroundTween	.Update(deltaTime);
	m_itemAlphaTween	.Update(deltaTime);
	m_guideAlphaTween	.Update(deltaTime);

	m_sprites[static_cast<UINT>(SpriteID::BACKGROUND_APLPHA)]->SetPosition(m_backgroundTween.GetValue());
	m_sprites[static_cast<UINT>(SpriteID::ITEM)]->SetOpacity(m_itemAlphaTween.GetValue());
	m_sprites[static_cast<UINT>(SpriteID::PUSHGUIDE_KEYBOARD)]->SetOpacity(m_guideAlphaTween.GetValue());

	// タスクの更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);

	auto kt = GetOwner()->GetCommonResources()->GetKeyboardTracker();

	if (m_backgroundTween.IsEnd() && kt->IsKeyReleased(Keyboard::Space))
	{
		GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_START);
		GetStateMachine()->ChangeState<NormalGameplayState>();
	}
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
}


