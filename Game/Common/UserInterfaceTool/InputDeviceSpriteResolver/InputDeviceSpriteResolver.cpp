/*****************************************************************//**
 * @file    InputDeviceSpriteResolver.h
 * @brief   UIガイド切り替え器に関するに関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/01/29
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputDeviceSpriteResolver.h"



// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連

// ゲームプレイロジック関連

// グラフィック関連

// ユーザーインターフェースツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"


// メンバ関数の定義 ===========================================================


/**
 * @brief コンストラクタ
 * 
 * @param[in] pKeyboard　	キーボード情報
 * @param[in] pGamePad		ゲームパッド情報
 */
InputDeviceSpriteResolver::InputDeviceSpriteResolver(const DirectX::Keyboard::KeyboardStateTracker* pKeyboard, const DirectX::GamePad::ButtonStateTracker* pGamePad)
	: m_pKeyboard{ pKeyboard }
	, m_pGamePad{ pGamePad }
	, m_isPreviousGamepadConnected{ false }
{
}

/**
 * @brief デストラクタ
 */
InputDeviceSpriteResolver::~InputDeviceSpriteResolver()
{

}


/**
 * @brief 更新処理
 *
 * @return なし
 */
void InputDeviceSpriteResolver::Update()
{

	// 接続状況に変化があれば
	if (m_isPreviousGamepadConnected != m_pGamePad->GetLastState().IsConnected())
	{
		// ゲームパッドに接続されているかどうかの取得
		bool isConnectedGamePad = m_pGamePad->GetLastState().IsConnected();

		// スプライトの切り替えをする
		SwitchSprite(isConnectedGamePad);

		// 結果を反映
		m_isPreviousGamepadConnected = isConnectedGamePad;
	}
	
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void InputDeviceSpriteResolver::Draw()
{

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void InputDeviceSpriteResolver::Finalize()
{

}

/**
 * @brief キーボード用スプライトの追加
 * 
 * @param[in] pSprite　追加スプライト
 */
void InputDeviceSpriteResolver::AddKeyboardSprite(Sprite* pSprite)
{
	m_keyboardSprites.push_back(pSprite);

	// 初期表示の設定
	pSprite->SetVisible(!m_isPreviousGamepadConnected);
	
}

/**
 * @brief ゲームパッド用スプライトの追加
 * 
 * 
 * @param[in] pSprite　追加スプライト
 */
void InputDeviceSpriteResolver::AddGamePadSprite(Sprite* pSprite)
{
	m_gamepadSprites.push_back(pSprite);

	// 初期表示の設定
	pSprite->SetVisible(m_isPreviousGamepadConnected);
	
}

/**
 * @brief キーボードスプライトの削除
 * 
 * @param[in] pSprite　削除するスプライト
 */
void InputDeviceSpriteResolver::RemoveKeyboardSprite(Sprite* pSprite)
{
	auto it = std::find(m_keyboardSprites.begin(), m_keyboardSprites.end(), pSprite);

	if (it != m_keyboardSprites.end())
	{
		m_keyboardSprites.erase(it);
	}
}

/**
 * @brief ゲームパッドスプライトの削除
 *
 * @param[in] pSprite　削除するスプライト
 */
void InputDeviceSpriteResolver::RemoveGamePadSprite(Sprite* pSprite)
{
	auto it = std::find(m_gamepadSprites.begin(), m_gamepadSprites.end(), pSprite);
	
	if (it != m_gamepadSprites.end())
	{
		m_gamepadSprites.erase(it);
	}

}

/**
 * @brief スプライトの切り替えをする
 * 
 * @param[in] isGamepadConnected
 */
void InputDeviceSpriteResolver::SwitchSprite(bool isGamepadConnected)
{
	// 表示する方のスプライトの取得
	std::vector<Sprite*>* visibleSprites = (isGamepadConnected) ? &m_gamepadSprites : &m_keyboardSprites;

	SetVisibleSprites(visibleSprites, true);

	// 表示しない方のスプライトの取得
	std::vector<Sprite*>* anotherSprites = (!isGamepadConnected) ? &m_gamepadSprites : &m_keyboardSprites;
	SetVisibleSprites(anotherSprites, false);

}

void InputDeviceSpriteResolver::SetVisibleSprites(std::vector<Sprite*>* sprites, bool isVisible)
{

	for (auto it = (*sprites).begin(); it != (*sprites).end();)
	{
		if (*it == nullptr)
		{
			it = sprites->erase(it);
		}
		else
		{
			(*it)->SetVisible(isVisible);

			it++;
		}

	}
}
