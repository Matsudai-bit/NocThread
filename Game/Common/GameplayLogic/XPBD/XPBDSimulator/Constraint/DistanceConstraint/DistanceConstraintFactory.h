/*****************************************************************//**
 * @file    DistanceConstraintFactory.h
 * @brief   距離制約の生成器に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include "../../../XPBDSimulator/Constraint/ConstraintFactory.h"

#include "../../../XPBDSimulator/XPBDSimulator.h"

// クラスの前方宣言 ===================================================
class SimParticle; // シミュレーションのパーティクル

class ICollider; // コライダーインターフェース
class CollisionManager; // 衝突管理
class IConstraint;

// クラスの定義 ===============================================================
/**
 * @brief 距離制約の生成器
 */
class DistanceConstraintFactory
	: public ConstraintFactoryBase
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	DistanceConstraintFactory();

	// デストラクタ
	~DistanceConstraintFactory();


// 操作
public:
	
	// 制約の作成
	std::vector<std::unique_ptr<IConstraint>> CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater) override;

// 取得/設定
public:

// 内部実装
private:


};
