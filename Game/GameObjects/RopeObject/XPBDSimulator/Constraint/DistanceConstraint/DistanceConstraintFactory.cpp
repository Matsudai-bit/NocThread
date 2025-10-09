/*****************************************************************//**
 * @file    DistanceConstraintFactory.h
 * @brief   衝突制約の生成器に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "DistanceConstraintFactory.h"

#include "DistanceConstraint.h"

#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

#include "Library/MyLib/MathUtils/MathUtils.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
DistanceConstraintFactory::DistanceConstraintFactory()
	: ConstraintFactory{ false }
{
}



/**
 * @brief デストラクタ
 */
DistanceConstraintFactory::~DistanceConstraintFactory()
{

}

/**
 * @brief 衝突制約の作成
 * 
 * @param[in] pParticle->パーティクル
 * 
 * @return 作成した衝突制約
 */
std::vector<std::unique_ptr<IConstraint>> DistanceConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles)
{
	std::vector<std::unique_ptr<IConstraint>> creationConstraints(pParticles->size()-1);

	for (size_t i = 0; i < pParticles->size(); i++)
	{
		if (i == pParticles->size() - 1) continue;

		// 処理用変数
		SimParticle* pSimParticle = &(*pParticles)[i].simP;

		// 初期化処理
		creationConstraints[i] = std::make_unique<DistanceConstraint>(pSimParticle, &(*pParticles)[i + 1].simP);
	}
	return creationConstraints;
	
}
