/*****************************************************************//**
 * @file    PauseGameplayState.h
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

// ライブラリ関連
#include "Library/DirectXFramework/RenderTexture.h"
#include "Library/MyLib/DirectXMyToolKit/DepthStencil/DepthStencil.h"

// フレームワーク関連
#include "Game/Common/Framework/StateMachine/StateBase/StateBase.h"
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"
#include "Game/Common/Framework/EventSystem/EventSystem.h"

// グラフィック関連
#include "Game/Common/Graphics/Line2D/Line2D.h"

// UI関連
#include "Game/UI/Pose/PoseMenu/PauseMenu.h"

// クラスの前方宣言 ===================================================
class GameplayScene; // ゲームプレイシーン
class Sprite;		 // スプライト
class Canvas;		 // キャンバス
class PauseMenu;		// ポーズメニュー
class TutorialWindow;	// チュートリアルウィンドウ

// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーンのポーズ状態
 */
class PauseGameplayState
	: public StateBase<GameplayScene>
{
// クラス定数の宣言 -------------------------------------------------
public:


	// --- 座標オフセット関連 ---
	static constexpr float FONT_PAUSE_POS_X_OFFSET		= 160.0f;	///< 「PAUSE」フォントの左端からのX座標オフセット
	static constexpr float FONT_PAUSE_POS_Y_OFFSET		= 100.0f;	///< 「PAUSE」フォントの下端からのY座標オフセット
	static constexpr float FONT_OPERATING_POS_X_OFFSET	= 450.0f;	///< 「操作説明」フォントの右端からのX座標オフセット
	static constexpr float FONT_OPERATING_POS_Y_OFFSET	= 80.0f;	///< 「操作説明」フォントの上端からのY座標オフセット
	static constexpr float OPERATING_UI_POS_X_OFFSET	= 300.0f;	///< 操作説明UIの右端からのX座標オフセット
	static constexpr float OPERATING_UI_POS_Y_OFFSET	= 40.0f;	///< 操作説明UIの中心からのY座標オフセット
	static constexpr float MANUAL_UI_POS_X_OFFSET		= 200.0f;	///< マニュアルUIの左端からのX座標オフセット
	static constexpr float MANUAL_UI_POS_Y_OFFSET		= 30.0f;	///< マニュアルUIの下端からのY座標オフセット


	// --- スプライトスケール関連 ---
	static constexpr float ALPHA_SPRITE_SCALE				= 1.4f;		///< 背景アルファスプライトの基本スケール
	static constexpr float FONT_SPRITE_SCALE				= 0.35f;	///< フォントスプライトの基本スケール
	static constexpr float OPERATING_SPRITE_SCALE_PC		= 0.275f;	///< 操作説明スプライト (PC) の基本スケール
	static constexpr float OPERATING_SPRITE_SCALE_GAMEPAD	= 0.65f;	///< 操作説明スプライト (ゲームパッド) の基本スケール
	static constexpr float MANUAL_SPRITE_SCALE				= 0.12f;	///< マニュアルスプライトの基本スケール


	// --- ライン描画関連 ---
	static constexpr float LINE0_ANGLE_DEGREE	= 70.0f;	///< 1本目のラインの角度 (度)
	static constexpr float LINE1_ANGLE_DEGREE	= 60.0f;	///< 2本目のラインの角度 (度)
	static constexpr float LINE0_POS_X_OFFSET	= 400.0f;	///< 1本目のラインのX座標中心からのオフセット
	static constexpr float LINE1_POS_X_OFFSET	= 100.0f;	///< 2本目のラインのX座標中心からのオフセット
	static constexpr float LINE_LENGTH			= 1000.0f;	///< ラインの長さ
	static constexpr float LINE_THICKNESS		= 2.0f;		///< ラインの太さ

// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<EventSystem<PauseMenu::MenuItem>> m_onPushMenuItemEvent; // メニューアイテムが押された時のイベント


	// スプライト
	std::unique_ptr<Sprite> m_backgroundAlphaSprite;	///< 背景透過用スプライト
	std::unique_ptr<Sprite> m_pauseFontSprite;			///< ポーズフォントのスプライト
	std::unique_ptr<Sprite> m_operatingFontSprite;		///< 操作方法フォントのスプライト
	std::unique_ptr<Sprite> m_keyboardOperatingSprite;			///< 操作方法のスプライト
	std::unique_ptr<Sprite> m_gamePadOperatingSprite;			///< 操作方法のスプライト
	std::unique_ptr<Sprite>	m_keyboardManualSprite;			///< 操作説明スプライト(キーボード）
	std::unique_ptr<Sprite>	m_gamePadManualSprite;			///< 操作説明スプライト(ゲームパッド）

	std::unique_ptr<Sprite> m_backInGameplayingSprite; ///< ゲームプレイ中の画像スプライト

	std::vector< std::unique_ptr<Line2D>>	m_lines;	///< 2D線

	std::unique_ptr<PauseMenu>				m_pauseMenu;	///< ポーズメニュー

	std::unique_ptr<TutorialWindow>			m_tutorialWindow; ///< チュートリアルウィンドウ

	std::unique_ptr<DX::RenderTexture>	m_pauseBackRT;	///< レンダーテクスチャ	(ポーズ画面の背景のSRVを取得するのに使用)

	// 簡易フラグ
	bool m_isPrevConnectedGamepad;  ///< 直前フレームでゲームパッドが接続されていたかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PauseGameplayState();

	// デストラクタ
	~PauseGameplayState();


// 操作
public:
	// 開始処理
	void OnStartState() override;

	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;

	// 終了処理
	void OnExitState() override;




// 取得/設定
public:


// 内部実装
private:
	// メニューアイテムを押した時に呼ばれる
	void OnPushMenuItem(PauseMenu::MenuItem menuItem);
	// ゲームに戻る
	void ContinueGame();
	// チュートリアルウィンドウを閉じる際の処理
	void OnCloseTutorialWindow();

	// 入力のコールバックの登録
	void RegisterBindCallbackToInput();
	// 紐づけの解除をする
	void UnBindCallbackToInput();

	// 左入力された時に呼ばれる
	void OnInputLeft(InputEventData data);
	// 右入力された時に呼ばれる
	void OnInputRight(InputEventData data);
	// 上入力された時に呼ばれる
	void OnInputUp(InputEventData data);
	// 下入力された時に呼ばれる
	void OnInputDown(InputEventData data);
	// 決定入力された時に呼ばれる
	void OnInputConfirm(InputEventData data);
	// 戻るが入力された時に呼ばれる
	void OnInputExit(InputEventData data);

	// メニューアイテムイベントの作成
	void CreateMenuItemEvent();

	// スプライトの作成
	void CreateSprites();
	// 2Dラインの作成
	void Create2DLines();
};
