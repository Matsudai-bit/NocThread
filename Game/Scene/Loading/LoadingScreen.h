/**
 * @file   LoadingScreen.h
 *
 * @brief  ローディング画面に関するヘッダファイル
 *
 * @author 松下大暉
 *
 * @date   2025/08/03
 */

// 多重インクルードの防止 =====================================================
#pragma once

#include <memory>


// ヘッダファイルの読み込み ===================================================
#include "Game/Manager/SceneManager/SceneManager.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/SpriteAnimator/SpriteAnimator.h"

// クラスの宣言 ===============================================================
class CommonResources;
class Canvas;

// クラスの定義 ===============================================================
/**
 * @brief ローディング画面
 */
class LoadingScreen : public MyLib::LoadingScreen<CommonResources>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	std::unique_ptr<Canvas> m_canvas; ///< UI表示キャンバス

	// オブジェクト関連

	// 背景
	std::unique_ptr<Sprite>				m_backgroundSprite;	///< 背景スプライト


	// UI
	std::unique_ptr<Sprite>				m_loadingSprite;	///< ロード中のスプライト
	std::unique_ptr<Sprite>				m_loadingFontSprite;///< ロード中のフォントスプライト

	std::unique_ptr<SpriteAnimator>		m_loadingAnimator;	///< ロードアニメーション
	std::unique_ptr<SpriteAnimator>		m_loadingFontAnimator;	///< ロードアニメーション



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	LoadingScreen();

	// デストラクタ
	~LoadingScreen();


// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;


// 取得/設定
public:


// 内部実装
private:



};
