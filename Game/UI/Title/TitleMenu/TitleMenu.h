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

#include "Game/Common/Line2D/Line2D.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

// クラスの前方宣言 ===================================================
class CommonResources;  // 共通リソース
class Canvas;			// キャンバス
class Sprite;			// スプライト

// クラスの定義 ===============================================================
/**
 * @brief タイトルメニュー
 */
class TitleMenu
{
// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float MAX_SELECTOR_LENGTH = 400.0f;
	static constexpr float EASING_TIME = 1.0f;

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
