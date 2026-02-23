/*****************************************************************//**
 * @file    DistanceConstraint.h
 * @brief   距離制約に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/14
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "DistanceConstraint.h"

#include "../../../XPBDSimulator/SimParticle/SimParticle.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================

/**
 * @brief 
 * 
 * @param[in] pP1 繋ぐパーティクル１
 * @param[in] pP2 繋ぐパーティクル2
 */
DistanceConstraint::DistanceConstraint(SimParticle* pP1, SimParticle* pP2)
	: m_constraint{}
{
	Initialize(pP1, pP2);
}

DistanceConstraint::DistanceConstraint()
	: m_connectParticle{}
	, m_constraint{}
{
}



/**
 * @brief デストラクタ
 */
DistanceConstraint::~DistanceConstraint()
{

}

/**
 * @brief 初期化処理
 * 
 * @param[in] pP1 繋ぐパーティクル１
 * @param[in] pP2 繋ぐパーティクル2
 */
void DistanceConstraint::Initialize(SimParticle* pP1, SimParticle* pP2)
{
	// 初期化
	m_constraint = ConstraintParam();

	m_connectParticle.pP1 = pP1;
	m_connectParticle.pP2 = pP2;

	DirectX::SimpleMath::Vector3 dirVec = pP1->GetX() - pP2->GetX();

	// 初期距離を設定
	m_constraint.startDistance = dirVec.Length();
}

/**
 * @brief パラメータをリセットする
 * 
 * @param[in] flexibility　柔軟性
 */
void DistanceConstraint::ResetConstraintParam(float flexibility)
{
	m_constraint.λ = 0.0f;
	m_constraint.α = flexibility;
}

float DistanceConstraint::EvaluateConstraint() const
{
	using namespace SimpleMath;

	const auto& pP1 = m_connectParticle.pP1;
	const auto& pP2 = m_connectParticle.pP2;

	auto vector = pP2->GetXi() - pP1->GetXi();
	auto distance = vector.Length();

	// 現在の距離誤差
	return distance - m_constraint.startDistance;
}

float DistanceConstraint::ComputeLambdaCorrection(float dt, float C) const
{
	const auto& pP1 = m_connectParticle.pP1;
	const auto& pP2 = m_connectParticle.pP2;

	auto vector = pP2->GetXi() - pP1->GetXi();
	auto distance = vector.Length();

	// 柔らかさのスケーリング (時間で正規化)
	auto αTilda = m_constraint.α / (dt * dt);

	if (distance < 1e-6f) return 0.0f;

	/// 単位方向ベクトル (制約勾配)
	auto deltaC = vector / distance; // ΔC

	// 合計の逆質量
	float invM = pP1->GetInvMass() + pP2->GetInvMass();

	// 有効質量 + 柔軟性による安定化項
	return (C - αTilda * m_constraint.λ) / (deltaC.Dot(invM * deltaC) + αTilda);
}

DirectX::SimpleMath::Vector3 DistanceConstraint::ComputeDeltaX(float deltaLambda) const
{

	const auto& pP1 = m_connectParticle.pP1;
	const auto& pP2 = m_connectParticle.pP2;

	/// 単位方向ベクトル (制約勾配)
	auto deltaC = (pP2->GetXi() - pP1->GetXi()); // ΔC
	deltaC.Normalize();
	return deltaC * deltaLambda;
}

void DistanceConstraint::ApplyPositionCorrection(float deltaLambda)
{
	DirectX::SimpleMath::Vector3 deltaX = ComputeDeltaX(deltaLambda); // deltaXは片方のパーティクルに対する補正量
	m_connectParticle.pP1->SetXi(m_connectParticle.pP1->GetXi() + m_connectParticle.pP1->GetInvMass() * deltaX);
	m_connectParticle.pP2->SetXi(m_connectParticle.pP2->GetXi() - m_connectParticle.pP2->GetInvMass() * deltaX);

}

