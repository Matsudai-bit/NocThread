/*****************************************************************//**
 * @file    PauseMenu.h
 * @brief   ポーズメニューに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/14
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include <functional>

#include "Game/Common/Line2D/Line2D.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

#include "Game/Common/Input/InputSystem/InputSystem.h"
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"

// クラスの前方宣言 ===================================================
class CommonResources;  // 共通リソース
class Canvas;			// キャンバス
class Sprite;			// スプライト

// クラスの定義 ===============================================================
/**
 * @brief ポーズメニュー
 */
class PauseMenu
{
// クラス定数の宣言 -------------------------------------------------
public:

	// --- ファイルパス関連 ---
	static constexpr const char* TEXTURE_PATH_CONTINUE		= "Pose/pose_font_continue.dds";	///< 「CONTINUE」フォントテクスチャのファイルパス
	static constexpr const char* TEXTURE_PATH_TUTORIAL		= "Pose/pose_font_tutorial.dds";	///< 「TUTORIAL」フォントテクスチャのファイルパス
	static constexpr const char* TEXTURE_PATH_SETTING		= "Pose/pose_font_setting.dds";		///< 「SETTING」フォントテクスチャのファイルパス
	static constexpr const char* TEXTURE_PATH_RETURN_TITLE	= "Pose/pose_font_returnTitle.dds"; ///< 「RETURN_TITLE」フォントテクスチャのファイルパス

	// --- スプライトスケール関連 ---
	static constexpr float FONT_SPRITE_SCALE = 0.30f;	///< メニューフォントの基本スケール

	// --- 座標・余白関連 ---
	static constexpr float MENU_MARGIN_X		= 70.0f;	///< メニューアイテム間のX方向マージンオフセット
	static constexpr float MENU_MARGIN_Y		= 00.0f;	///< メニューアイテム間のY方向マージンオフセット
	static constexpr float FIXED_POS_X_OFFSET	= 350.0f;	///< メニュー全体の左端からの固定X座標オフセット
	static constexpr float FIXED_POS_Y_OFFSET	= 10.0f;	///< メニュー全体の中心からの固定Y座標オフセット
	static constexpr float RETURN_TITLE_OFFSET_X= 240.0f;	///< 「RETURN_TITLE」アイテムの特殊X座標オフセット
	static constexpr float RETURN_TITLE_OFFSET_Y= 450.0f;	///< 「RETURN_TITLE」アイテムの特殊Y座標オフセット


	// --- セレクターアニメーション・描画関連 ---
	static constexpr float EASING_TIME			= 1.0f;		///< セレクターアニメーションのイージング時間 (秒)
	static constexpr float MAX_SELECTOR_LENGTH	= 500.0f;	///< セレクターの最大長さの仮定義

	static constexpr float SELECTOR_WIDTH_OFFSET	= 20.0f;	///< セレクターの長さに追加される幅オフセット
	static constexpr float SELECTOR_MIN_LENGTH_RATIO= 0.2f;		///< セレクターの最小長さの比率 (MAX_SELECTOR_LENGTHに対する割合)
	static constexpr float SELECTOR_CURSOR_Y_OFFSET = 30.0f;	///< セレクターのY座標調整オフセット
	static constexpr float SELECTOR_LINE_THICKNESS	= 2.0f;		///< セレクターとして描画するラインの太さ

// 列挙型の宣言　 -------------------------------------------------
public:

	/**
	 * @brief メニューのアイテム
	 */
	enum class MenuItem
	{

		CONTINUE = 0,
		TUTORIAL,
		SETTING,
		RETURN_TITLE,

		NUM
	};


// データメンバの宣言 -----------------------------------------------
private:

	// フォントスプライト
	std::vector< std::unique_ptr<Sprite>> m_pauseFontSprites;

	std::function<void(MenuItem)>  m_pushButtonFunc;

	int m_currentSelectItemForInt;	///< 現在選択しているアイテム

	const CommonResources* m_pCommonResources;	///< 共通リソース

	Line2D	m_line;

	ElapsedTimeCounter m_ElapsedTimeCounter; ///< 経過時間カウンター

	std::unique_ptr<InputSystem<InputActionType::UIActionID>> m_uiInput; ///< UI入力


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PauseMenu();

	// デストラクタ
	~PauseMenu();


// 操作
public:
	// 初期化処理
	void Initialize(Canvas* pCanvas, const CommonResources* pCommonResources, std::function<void(MenuItem)> pushButtonFunc);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();


// 取得/設定
public:


// 内部実装
private:

	// 下にセレクターが動くことが出来るかどうか
	bool CanMoveDownSelector() const;
	// 上にセレクターが動くことが出来るかどうか
	bool CanMoveUpSelector() const;
	// 選択することができるかどうか
	bool CanPush() const;


};
