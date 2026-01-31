/*****************************************************************//**
 * @file    TitleMenu.h
 * @brief   タイトルメニューに関するヘッダーファイル
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

// フレームワーク関連
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

// グラフィック関連
#include "Game/Common/Graphics/Line2D/Line2D.h"

// ユーティリティ関連
#include "Game/Common/Utillities/ElapsedTimeCounter/ElapsedTimeCounter.h"

// クラスの前方宣言 ===================================================
class CommonResources;  // 共通リソース
class Canvas;			// キャンバス
class Sprite;			// スプライト
struct InputEventData;  // 入力イベントデータ
// クラスの定義 ===============================================================
/**
 * @brief タイトルメニュー
 */
class TitleMenu
{
// クラス定数の宣言 -------------------------------------------------
public:


	// --- スプライトスケール関連 ---
	static constexpr float FONT_SPRITE_SCALE = 0.35f;	///< メニューフォントの基本スケール

	// --- 座標・余白関連 ---
	static constexpr float MENU_MARGIN_X		= 280.0f;	///< メニュー全体の左端からのX座標オフセット
	static constexpr float MENU_MARGIN_Y		= 20.0f;	///< メニューアイテム間のY方向マージン
	static constexpr float FIXED_POS_Y_OFFSET	= 30.0f;	///< メニュー全体に適用されるY座標調整オフセット


	// --- セレクターアニメーション・描画関連 ---
	static constexpr float EASING_TIME			= 1.0f;		///< セレクターアニメーションのイージング時間 (秒)
	static constexpr float MAX_SELECTOR_LENGTH	= 500.0f;	///< セレクターの最大長さの仮定義

	static constexpr float SELECTOR_WIDTH_OFFSET		= 20.0f;	///< セレクターの長さに追加される幅オフセット
	static constexpr float SELECTOR_MIN_LENGTH_RATIO	= 0.2f;		///< セレクターの最小長さの比率 (MAX_SELECTOR_LENGTHに対する割合)
	static constexpr float SELECTOR_CURSOR_Y_OFFSET		= 30.0f;	///< セレクターのY座標調整オフセット
	static constexpr float SELECTOR_LINE_THICKNESS		= 2.0f;		///< セレクターとして描画するラインの太さ

// 列挙型の宣言　 -------------------------------------------------
public:

	/**
	 * @brief メニューのアイテム
	 */
	enum class MenuItem
	{

		PLAY = 0,
		TUTORIAL,
		SETTING,
		QUIT,

		NUM
	};


// データメンバの宣言 -----------------------------------------------
private:

	// フォントスプライト
	std::vector< std::unique_ptr<Sprite>> m_titleFontSprites;

	std::function<void(MenuItem)>  m_pushButtonFunc;

	int m_currentSelectItemForInt;	///< 現在選択しているアイテム

	const CommonResources* m_pCommonResources;	///< 共通リソース

	Line2D	m_line;

	ElapsedTimeCounter m_ElapsedTimeCounter; ///< 経過時間カウンター

	bool m_isActive; ///< 活動しているかどうか


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TitleMenu();

	// デストラクタ
	~TitleMenu();


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

	// 上にセレクターが動かす
	void OnMoveUpSelector	(const InputEventData& data);
	// 下にセレクターが動かす
	void OnMoveDownSelector	(const InputEventData& data);
	// 選択する
	void OnSelect			(const InputEventData& data) ;


// 取得/設定
public:

	void SetActive(bool isActive) { m_isActive = isActive; }
	bool IsActive() const { return m_isActive; }


// 内部実装
private:

	
};
