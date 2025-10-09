/*****************************************************************//**
 * @file    IdleStageObjectState.h
 * @brief   ステージオブジェクトの待機状態に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/StateMachine/StateBase/StateBase.h"

// クラスの前方宣言 ===================================================
class StageObject;

// クラスの定義 ===============================================================
/**
 * @brief ステージオブジェクトの待機状態
 */

class IdleStageObjectState : 
	public StateBase<StageObject>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	IdleStageObjectState();

	// デストラクタ
	~IdleStageObjectState();


// 操作
public:

	// 開始処理
	void OnStartState() override;

	// 終了処理
	void OnExitState() override;

	// 更新処理
	void OnUpdate(float elapsedTime) override;

	// 描画処理
	void OnDraw() override;


// 取得/設定
public:


// 内部実装
private:


};
