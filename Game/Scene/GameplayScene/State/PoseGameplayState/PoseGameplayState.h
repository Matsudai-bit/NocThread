/*****************************************************************//**
 * @file    PoseGameplayState.h
 * @brief   ゲームプレイシーンのポーズ状態に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include "Game/Common/StateMachine/StateBase/StateBase.h"
#include "Game/Common/Line2D/Line2D.h"
#include "Game/UI/Pose/PoseMenu/PoseMenu.h"


// クラスの前方宣言 ===================================================
class GameplayScene; // ゲームプレイシーン
class Sprite;		 // スプライト
class Canvas;		 // キャンバス
class PoseMenu;		// ポーズメニュー
class TutorialWindow;	// チュートリアルウィンドウ

// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーンのポーズ状態
 */
class PoseGameplayState
	: public StateBase<GameplayScene>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	// スプライト
	std::unique_ptr<Sprite> m_backgroundAlphaSprite;	///< 背景透過用スプライト
	std::unique_ptr<Sprite> m_poseFontSprite;			///< ポーズフォントのスプライト
	std::unique_ptr<Sprite> m_operatingFontSprite;		///< 操作方法フォントのスプライト
	std::unique_ptr<Sprite> m_operatingSprite;			///< 操作方法のスプライト

	std::unique_ptr<Canvas> m_canvas;					///< キャンバス　

	std::vector< std::unique_ptr<Line2D>>	m_lines;	///< 2D線

	std::unique_ptr<PoseMenu>				m_poseMenu;	///< ポーズメニュー

	std::unique_ptr<TutorialWindow>			m_tutorialWindow; ///< チュートリアルウィンドウ

	// 簡易フラグ
	bool m_isDisplayingTutorialWindow;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PoseGameplayState();

	// デストラクタ
	~PoseGameplayState();


// 操作
public:
	// 開始処理
	void OnStartState() override;

	// 更新処理
	void OnUpdate(float elapsedTime) override;

	// 描画処理
	void OnDraw() override;




// 取得/設定
public:


// 内部実装
private:
	// メニューアイテムを押した時に呼ばれる
	void OnPushMenuItem(PoseMenu::MenuItem menuItem);

	// ゲームに戻る
	void ContinueGame();


	// チュートリアルウィンドウを閉じる際の処理
	void OnCloseTutorialWindow();
};
