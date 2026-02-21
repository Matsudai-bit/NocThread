/*****************************************************************//**
 * @file    InputDeviceSpriteResolver.h
 * @brief   UIガイド切り替え器に関するヘッダーファイル
 * 
 *	入力デバイス毎のUIの表記を変える
 *
 * @author  松下大暉
 * @date    2025/01/29
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>

// クラスの前方宣言 ===================================================
class Sprite; // スプライト

// クラスの定義 ===============================================================
/**
 * @brief　入力デバイス毎のスプライトの表記を切り替え器
 */
class InputDeviceSpriteResolver
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::vector<Sprite*> m_keyboardSprites;
	std::vector<Sprite*> m_gamepadSprites;

	const DirectX::Keyboard::KeyboardStateTracker*	m_pKeyboard;
	const DirectX::GamePad::ButtonStateTracker*		m_pGamePad;

	bool m_isPreviousGamepadConnected;	///< 前フレームでゲームパッドが接続されているかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputDeviceSpriteResolver(const DirectX::Keyboard::KeyboardStateTracker* pKeyboard, const DirectX::GamePad::ButtonStateTracker* pGamePad);

	// デストラクタ
	~InputDeviceSpriteResolver();


// 操作
public:


	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();

	// キーボード用スプライトの追加
	void AddKeyboardSprite(Sprite* pSprite);
	// ゲームパッド用スプライトの追加
	void AddGamePadSprite (Sprite* pSprite);

	// キーボード用スプライトの削除
	void RemoveKeyboardSprite(Sprite* pSprite);
	// ゲームパッド用スプライトの削除
	void RemoveGamePadSprite(Sprite* pSprite);

// 取得/設定
public:


// 内部実装
private:

	// 表示の切り替えをする
	void SwitchSprite(bool isGamepadConnected);

	// 表示設定
	void SetVisibleSprites(std::vector<Sprite*>* sprites, bool isVisible);
};
