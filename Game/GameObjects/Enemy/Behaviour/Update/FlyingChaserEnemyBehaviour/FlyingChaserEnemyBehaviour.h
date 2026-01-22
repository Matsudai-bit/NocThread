/*****************************************************************//**
 * @file    FlyingChaserEnemyBehaviour.h
 * @brief   敵の空中追跡挙動に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/09
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// ゲームオブジェクト関連
#include "Game/GameObjects/Enemy/Behaviour/Update/IEnemyUpdateBehaviour.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/Behaviour/SteeringBehavior/SteeringBehavior.h"

// ユーティリティ関連
#include "Game/Common/Utillities/ElapsedTimeCounter/ElapsedTimeCounter.h"

// クラスの前方宣言 ===================================================
class Player;

// クラスの定義 ===============================================================
/**
 * @brief 敵の空中追跡挙動
 */
class FlyingChaserEnemyBehaviour  
	: public IEnemyUpdateBehaviour
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	ElapsedTimeCounter m_playerTargetTimeCounter; ///< プレイヤーを補足するクールタイムの加算器

	DirectX::SimpleMath::Vector3 m_targetDirection; ///< 目標方向

	SteeringBehavior m_steeringBehavior;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	FlyingChaserEnemyBehaviour();

	// デストラクタ
	~FlyingChaserEnemyBehaviour();


// 操作
public:

	// 更新処理
	virtual void Update(Enemy* pEnemy, float deltaTime,const CommonResources* pCommonResources) override;



// 取得/設定
public:


// 内部実装
private:


};
