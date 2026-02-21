/*****************************************************************//**
 * @file    PauseGuideUI.cpp
 * @brief   ポーズ画面へのガイドUIに関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/01/31
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PauseGuideUI.h"



// ライブラリ

// ファクトリー関連

// データベース関連
#include "Game/Common/Database/TextureDatabase.h"

// フレームワーク関連
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"

// ゲームプレイロジック関連

// UI関連
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/InputDeviceSpriteResolver/InputDeviceSpriteResolver.h"

// グラフィック関連

#include "Game/Common/Screen.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================

/**
 * @brief コンストラクタ
 * 
 * @param[in] pResourceManager			 リソース管理
 * @param[in] pInputDeviceSpriteResolver 入力デバイス毎の切り替え器
 */
PauseGuideUI::PauseGuideUI(ResourceManager* pResourceManager, InputDeviceSpriteResolver* pInputDeviceSpriteResolver)
	: m_pInputDeviceSpriteResolver{ pInputDeviceSpriteResolver }
{
	auto screen = Screen::Get();

	m_keyboardManualSprite	= std::make_unique<Sprite>();
	m_gamePadManualSprite	= std::make_unique<Sprite>();

	// スプライトのテクスチャ設定
	m_keyboardManualSprite->Initialize(pResourceManager->CreateTexture(
		TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_INGAME_KEYBOARD)));
	m_gamePadManualSprite->Initialize(pResourceManager->CreateTexture(
		TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_INGAME_KEYBOARD)));

	// スプライトの座標設定
	m_keyboardManualSprite	->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + 200.0f * screen->GetScreenScale(), screen->GetBottomF() - 40.0f * screen->GetScreenScale()));
	m_gamePadManualSprite	->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + 200.0f * screen->GetScreenScale(), screen->GetBottomF() - 40.0f * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_keyboardManualSprite	->SetScale(0.2f * screen->GetScreenScale());
	m_gamePadManualSprite	->SetScale(0.2f * screen->GetScreenScale());

	// 入力デバイス毎の切り替え器の登録
	m_pInputDeviceSpriteResolver->AddKeyboardSprite(m_keyboardManualSprite.get());
	m_pInputDeviceSpriteResolver->AddGamePadSprite(m_gamePadManualSprite.get());
}



/**
 * @brief デストラクタ
 */
PauseGuideUI::~PauseGuideUI()
{
	if (m_pInputDeviceSpriteResolver)
	{
		// 入力デバイス毎の切り替え器から削除
		m_pInputDeviceSpriteResolver->RemoveKeyboardSprite(m_keyboardManualSprite.get());
		m_pInputDeviceSpriteResolver->RemoveGamePadSprite(m_gamePadManualSprite.get());
	}
	
}


void PauseGuideUI::DrawSprite(DirectX::SpriteBatch* pSpriteBatch)
{
	m_keyboardManualSprite	->DrawSprite(pSpriteBatch);
	m_gamePadManualSprite	->DrawSprite(pSpriteBatch);
}

