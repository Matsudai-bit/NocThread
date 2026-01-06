/*****************************************************************//**
 * @file    ConstraintFactoryBase.h
 * @brief   制約のに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

#include "Game/GameObjects/RopeObject/XPBDSimulator/XPBDSimulator.h"

// クラスの前方宣言 ===================================================
class IConstraint;
class SimParticle;



// クラスの定義 ===============================================================
/**
 * @brief 制約の生成
 */
class ConstraintFactoryBase
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	bool m_isDynamic; ///< 毎フレーム制約の更新をかけるかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ConstraintFactoryBase(bool isDynamic);

	// デストラクタ
	virtual ~ConstraintFactoryBase() = default;

	// 制約の作成
	virtual std::vector<std::unique_ptr<IConstraint>> CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater)  = 0;

	// リセット
	virtual void Reset() {};


// 取得/設定
public:

	bool IsDynamic() const { return m_isDynamic; };


};
