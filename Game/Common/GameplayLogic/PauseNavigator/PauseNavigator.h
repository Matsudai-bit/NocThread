/*****************************************************************//**
 * @file    PauseNavigator.h
 * @brief   ポーズ画面へのナビゲーターに関するヘッダーファイル
 *	
 *		ポーズ画面のガイドUI、入力判定を行う
 * 
 * @author  松下大暉
 * @date    2025/01/31
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <functional>

// クラスの前方宣言 ===================================================
class Sprite;					// スプライト
class ResourceManager;			// リソース管理
class InputDeviceSpriteResolver;// 入力デバイス毎の切り替え器
class PauseGuideUI;				// ポーズガイドUI
class Canvas;					// キャンバス
class InputManager;				// 入力管理
struct InputEventData;

// クラスの定義 ===============================================================
/**
 * @brief ポーズ画面へのナビゲーター
 */
class PauseNavigator
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<PauseGuideUI> m_pauseGuideUI;	///< ポーズガイドUI

	Canvas* m_pCanvas; ///< キャンバス

	InputManager* m_pInputManager; ///< 入力管理

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PauseNavigator();

	// デストラクタ
	~PauseNavigator();


// 操作
public:
	// 初期化処理
	void Initialize(
		ResourceManager* pResourceManager, 
		InputDeviceSpriteResolver* pInputDeviceSpriteResolver, 
		Canvas* pCanvas,
		InputManager* pInputManager);

	// ポーズ画面への入力コールバックの設定
	void SetInputCallback(const std::function<void(const InputEventData&)>& callback);



// 取得/設定
public:


// 内部実装
private:


};
