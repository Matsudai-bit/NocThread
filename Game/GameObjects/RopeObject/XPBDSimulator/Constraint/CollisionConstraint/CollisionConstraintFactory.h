/*****************************************************************//**
 * @file    CollisionConstraintFactory.h
 * @brief   衝突制約の生成器に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/ConstraintFactory.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/XPBDSimulator.h"

// クラスの前方宣言 ===================================================
class SimParticle; // シミュレーションのパーティクル

class ICollider; // コライダーインターフェース
class CollisionManager; // 衝突管理
class IConstraint;

// クラスの定義 ===============================================================
/**
 * @brief 衝突制約の生成器
 */
class CollisionConstraintFactory
	: public ConstraintFactoryBase
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	CollisionManager* m_pCollisionManager; ///< 衝突管理

	XPBDSimulator::Parameter m_paramater;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionConstraintFactory(CollisionManager* pCollisionManager, XPBDSimulator::Parameter paramater);

	// デストラクタ
	~CollisionConstraintFactory();


// 操作
public:
	
	// 制約の作成
	std::vector<std::unique_ptr<IConstraint>> CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles) override;

// 取得/設定
public:

// 内部実装
private:


};
