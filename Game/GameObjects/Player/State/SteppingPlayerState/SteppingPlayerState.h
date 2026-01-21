/*****************************************************************//**
 * @file    SteppingPlayerState.h
 * @brief   ステップ状態のプレイヤーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/08
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/StateMachine/StateBase/StateBase.h"

#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

#include "Game/Common/GameEffect/Effects/ConcentrationLines/ConcentrationLines.h"
#include "Game/Common/GameEffect/GameEffectController.h"

// クラスの前方宣言 ===================================================
class Player;

// クラスの定義 ===============================================================
/**
 * @brief ジャンプ状態のプレイヤー
 */
class SteppingPlayerState : public StateBase<Player>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector3 m_impulseDirection;

	ElapsedTimeCounter				m_targetCounter; ///< 目標時間までの加算器

	float m_currentLerpValue;	///< 現在の補間量

	DirectX::SimpleMath::Vector3	m_targetPosition; ///< 目標座標
	DirectX::SimpleMath::Vector3	m_startPosition; ///< 開始座標

	ConcentrationLines* m_pConcentrationLines; ///< 集中線エフェクト
	int m_effectId; ///< エフェクトID

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SteppingPlayerState();

	// デストラクタ
	~SteppingPlayerState();


// 操作
public:

	// 状態開始時に呼ばれる
	void OnStartState() override;


	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;
	void OnExitState() override;




// 取得/設定
public:


// 内部実装
private:


};
