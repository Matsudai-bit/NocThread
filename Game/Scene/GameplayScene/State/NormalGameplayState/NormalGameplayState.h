/*****************************************************************//**
 * @file    NormalGameplayState.h
 * @brief   ゲームプレイシーンの通常状態に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>

// フレームワーク関連
#include "Game/Common/Framework/StateMachine/StateBase/StateBase.h"
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

// クラスの前方宣言 ===================================================
class GameplayScene;	// ゲームプレイシーン
class Canvas;			// キャンバス
class Sprite;
class PauseNavigator;	// ポーズナビゲーター
struct InputEventData;	// 入力イベント時のデータ
// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーンの通常状態
 */
class NormalGameplayState
	: public StateBase<GameplayScene>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	
	std::unique_ptr<PauseNavigator> m_pauseNavigator;	///< ポーズナビゲーター


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	NormalGameplayState();

	// デストラクタ
	~NormalGameplayState();


// 操作
public:
	// 状態の開始
	void OnStartState() override;

	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;




// 取得/設定
public:


// 内部実装
private:

	// ナビゲーターの作成
	void CreatePauseNavigator();

	// ポーズ画面を開く処理
	void OnOpenPause(const InputEventData& data);
};
