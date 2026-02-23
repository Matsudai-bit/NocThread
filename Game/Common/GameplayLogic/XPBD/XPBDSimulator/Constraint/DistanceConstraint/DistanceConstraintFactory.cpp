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


// ライブラリ関連
#include "Library/MyLib/MathUtils/MathUtils.h"

// フレームワーク関連
#include "Game/Common/Framework/Collision/CollisionManager/CollisionManager.h"

// ユーティリティ関連
#include "../../Constraint/CollisionConstraint/CollisionConstraint.h"
#include "../../SimParticle/SimParticle.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
DistanceConstraintFactory::DistanceConstraintFactory()
	: ConstraintFactoryBase{ false }
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
std::vector<std::unique_ptr<IConstraint>> DistanceConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater)
{
	UNREFERENCED_PARAMETER(paramater);
	int particleNum = static_cast<int>(pParticles->size() - 1);
	if (particleNum <= 0) {
		return std::vector<std::unique_ptr<IConstraint>>();}
	std::vector<std::unique_ptr<IConstraint>> creationConstraints(particleNum);

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
