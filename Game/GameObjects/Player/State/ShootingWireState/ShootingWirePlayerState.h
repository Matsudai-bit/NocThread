/*****************************************************************//**
 * @file    ShootingWirePlayerState.h
 * @brief   ワイヤー発射状態のプレイヤーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/21
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/StateMachine/StateBase/StateBase.h"


// クラスの前方宣言 ===================================================
class Player;
class GameObject;

// クラスの定義 ===============================================================
/**
 * @brief ワイヤー発射状態のプレイヤー
 */
class ShootingWirePlayerState : public StateBase<Player>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	bool m_isJumping;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ShootingWirePlayerState();

	// デストラクタ
	~ShootingWirePlayerState();


// 操作
public:

	// 状態開始時に呼ばれる
	void OnStartState() override;


	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;

	// ワイヤーが衝突した時によばれる
	void OnCollisionWire(const GameObject* pHitObject);

	// 状態終了時に呼ばれる
	void OnExitState() override;

// 取得/設定
public:


// 内部実装
private:


};
