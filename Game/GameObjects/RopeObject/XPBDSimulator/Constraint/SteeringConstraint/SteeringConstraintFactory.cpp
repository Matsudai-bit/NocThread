/*****************************************************************//**
 * @file    SteeringConstraintFactory.h
 * @brief   衝突制約の生成器に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SteeringConstraintFactory.h"

#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/SteeringConstraint/SteeringConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

#include "Library/MyLib/MathUtils/MathUtils.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
SteeringConstraintFactory::SteeringConstraintFactory(XPBDSimulator::Parameter paramater)
	: ConstraintFactoryBase		{ true }
	, m_paramater			{ paramater }
{
}



/**
 * @brief デストラクタ
 */
SteeringConstraintFactory::~SteeringConstraintFactory()
{

}

/**
 * @brief 衝突制約の作成
 * 
 * @param[in] pParticle->パーティクル
 * 
 * @return 作成した衝突制約
 */
std::vector<std::unique_ptr<IConstraint>> SteeringConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles)
{
	std::vector<std::unique_ptr<IConstraint>> creationConstraints;

	for (auto& particle : *pParticles)
	{
		creationConstraints.emplace_back(std::move(std::make_unique<SteeringConstraint>(&particle.simP, particle.simP.GetX())));
	}
	return creationConstraints;
	
}
