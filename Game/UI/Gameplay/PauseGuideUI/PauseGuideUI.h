/*****************************************************************//**
 * @file    PauseGuideUI.h
 * @brief   ポーズ画面へのガイドUIに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2026/01/31
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/UserInterfaceTool/Sprite/ISprite2D.h"

// クラスの前方宣言 ===================================================
class Sprite;					// スプライト
class ResourceManager;			// リソース管理
class InputDeviceSpriteResolver;// 入力デバイス毎の切り替え器

// クラスの定義 ===============================================================
/**
 * @brief ポーズ画面へのガイドUI
 */
class PauseGuideUI
	: public ISprite2D
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<Sprite>	m_keyboardManualSprite;				///< 操作ガイドスプライト(キーボード）
	std::unique_ptr<Sprite>	m_gamePadManualSprite;				///< 操作ガイドスプライト（ゲームパッド）

	InputDeviceSpriteResolver* m_pInputDeviceSpriteResolver;	///< 入力デバイス毎の切り替え器

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PauseGuideUI(ResourceManager* pResourceManager, InputDeviceSpriteResolver* pInputDeviceSpriteResolver);

	// デストラクタ
	~PauseGuideUI();


// 操作
public:

	void DrawSprite(DirectX::SpriteBatch* pSpriteBatch) override;

// 取得/設定
public:
	bool IsVisible() const  override { return true; }


// 内部実装
private:


};
