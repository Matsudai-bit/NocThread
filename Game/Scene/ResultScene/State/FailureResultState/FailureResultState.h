/*****************************************************************//**
 * @file    FailureResultState.h
 * @brief   リザルトシーンの失敗状態に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// ライブラリ関連
#include "Library/MyLib/EasingKit/EasingKit.h"

// フレームワーク関連
#include "Game/Common/Framework/StateMachine/StateBase/StateBase.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/TimeUI/TimeUI.h"
#include "Game/Common/UserInterfaceTool/SpriteNumber/SpriteNumber.h"

// ユーティリティ関連
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

// クラスの前方宣言 ===================================================
class ResultScene;	// リザルトシーン
class Sprite;		// スプライト

// クラスの定義 ===============================================================
/**
 * @brief リザルトシーンの失敗状態
 */
class FailureResultState : public StateBase<ResultScene>
{
	// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float EASING_TIME = 2.5f;


	// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<Sprite> m_failureFontSprite;	///< 失敗の文字スプライト
	std::unique_ptr<Sprite> m_resultFontSprite;		///< リザルトの文字スプライト
	std::unique_ptr<Sprite> m_survivalTime;			///< 生存時間の文字スプライト
	std::unique_ptr<Sprite> m_destroyedNumFontSprite;///< 撃破数の文字スプライト

	std::unique_ptr<TimeUI> m_clearTime;			///< クリア時間UI
	std::unique_ptr<SpriteNumber> m_destroyedNumSprite;			///< 撃破数の数字スプライト

	// 演出関連
	ElapsedTimeCounter m_elapsedCounterForEasing;
	

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	FailureResultState();

	// デストラクタ
	~FailureResultState();


	// 操作
public:

	// 開始処理
	void OnStartState() override;

	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;




	// 取得/設定
public:


	// 内部実装
private:


};
