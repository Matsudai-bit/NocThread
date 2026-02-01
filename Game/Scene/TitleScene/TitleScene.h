/**
 * @file   TitleScene.h
 *
 * @brief  タイトルシーンに関するヘッダファイル
 *
 * @author 松下大暉
 *
 * @date   2025/04/06
 */

// 多重インクルードの防止 =====================================================
#pragma once

#include <memory>
#include <vector>


// ヘッダファイルの読み込み ===================================================
#include "Game/Scene/SceneManager.h"

// フレームワーク関連
#include "Game/Common/Framework/EventSystem/EventSystem.h"

// UIメニュー関連
#include "Game/UI/Title/TitleMenu/TitleMenu.h"

// グラフィック関連
#include "Game/Common/Graphics/Line2D/Line2D.h"

// ユーティリティ関連
#include "Game/Common/Utillities/ElapsedTimeCounter/ElapsedTimeCounter.h"
#include "Game/Common/Utillities/DxTween/DxTween.h"

// クラスの宣言 ===============================================================
class CommonResources;  // 共通リソース
class Canvas;			// キャンバス
class Sprite;			// スプライト
class TitleMenu;		// タイトルメニュー
struct InputEventData;	// 入力イベントデータ
class TutorialWindow;	// チュートリアルウィンドウ

// クラスの定義 ===============================================================
/**
 * @brief タイトルシーン
 */
class TitleScene : public MyLib::Scene<CommonResources>
{
// クラス定数の宣言 -------------------------------------------------
public:

	static constexpr float LOGO_EASING_TIME = 1.5f; ///< ロゴの透過アニメーションのイージング時間 (秒)

    // --- 座標オフセット関連 ---
    static constexpr float LOGO_POS_OFFSET_X	= 300.0f;	///< ロゴスプライトの右端からのX座標オフセット
    static constexpr float LOGO_POS_OFFSET_Y	= 150.0f;	///< ロゴスプライトの上端からのY座標オフセット
    static constexpr float MANUAL_POS_OFFSET_X	= 300.0f;	///< マニュアルスプライトの右端からのX座標オフセット
    static constexpr float MANUAL_POS_OFFSET_Y	= 50.0f;	///< マニュアルスプライトの下端からのY座標オフセット

    // --- スケール関連 ---
    static constexpr float BACKGROUND_SCALE = 0.68f;	///< 背景スプライトの基本スケール
    static constexpr float ALPHA_SCALE		= 1.68f;	///< アルファマスクスプライトの基本スケール
    static constexpr float LOGO_SCALE		= 0.68f;	///< ロゴスプライトの基本スケール
    static constexpr float MANUAL_SCALE		= 0.17f;	///< マニュアルスプライトの基本スケール

// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	std::unique_ptr<Canvas> m_canvas; ///< UI表示キャンバス
	std::unique_ptr<EventSystem<TitleMenu::MenuItem>> m_onPushMenuItemEvent; // メニューアイテムが押された時のイベント

	// オブジェクト関連

	// UI
	std::unique_ptr<Sprite>				m_backgroundSprite;		///< 背景スプライト

	std::unique_ptr<Sprite>				m_alphaSprite;			///< 透過フィルタースプライト
	std::unique_ptr<Sprite>				m_logoSprite;			///< ロゴスプライト
	std::unique_ptr<Sprite>				m_keyboardManualSprite;	///< 操作説明スプライト(キーボード）
	std::unique_ptr<Sprite>				m_gamePadManualSprite;	///< 操作説明スプライト(ゲームパッド）
	
	std::unique_ptr<TitleMenu>			m_titleMenu;		///< タイトルメニュー
	std::unique_ptr<TutorialWindow>		m_tutorialWindow;	///< チュートリアルウィンドウ

	// 演出系
	ElapsedTimeCounter m_ElapsedTimeCounter; ///< 経過時間カウンター
	DxTween<float>		m_logoOpacityTween;	///< ロゴの透過率のイージング

	// サウンド
	int m_bgmSoundID;

	// デバック（検証用）
	bool m_isPrevConnectedGamepad; ///< 直前フレームでゲームパッドが接続されていたかどうか
	bool m_isQuit;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TitleScene();

	// デストラクタ
	~TitleScene();


// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float deltaTime) override;

	// 描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;


// 取得/設定
public:


// 内部実装
private:


	// 中心の算出
	DirectX::SimpleMath::Vector2 CalcCenterOrigin(ID3D11ShaderResourceView* pTexture);

	// メニューアイテムを押した時に呼ばれる
	void OnPushMenuItem(TitleMenu::MenuItem menuItem);

	// チュートリアルウィンドウを閉じる際の処理
	void OnCloseTutorialWindow();

	// ロゴの不透明度を更新する
	void UpdateLogoOpacity(float deltaTime);

	// タイトルメニューの更新処理
	void UpdateTitleMenu(float deltaTime);

	// 入力のコールバックの登録
	void RegisterBindCallbackToInput();
	// 紐づけの解除をする
	void UnBindCallbackToInput();


	// 左入力された時に呼ばれる
	void OnInputLeft	(const InputEventData& data);
	// 右入力された時に呼ばれる
	void OnInputRight(const InputEventData& data);
	// 上入力された時に呼ばれる
	void OnInputUp(const InputEventData& data);
	// 下入力された時に呼ばれる
	void OnInputDown(const InputEventData& data);
	// 決定入力された時に呼ばれる
	void OnInputConfirm(const InputEventData& data);
	// 戻るが入力された時に呼ばれる
	void OnInputExit(const InputEventData& data);
	// メニューアイテムイベントの作成
	void CreateMenuItemEvent();

	// スプライトの作成
	void CreateSprites();
};
