/*****************************************************************//**
 * @file    CollisionConstraint.h
 * @brief   衝突制約に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/07/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Game/Common/Utillities/Collision/Collision.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
CollisionConstraint::CollisionConstraint(SimParticle* pParticle)
	: m_pParticle{ pParticle }
{

}



/**
 * @brief デストラクタ
 */
CollisionConstraint::~CollisionConstraint()
{

}

/**
 * @brief パラメータをリセットする
 *
 * @param[in] flexibility　柔軟性
 */
void CollisionConstraint::ResetConstraintParam(float flexibility)
{
	m_constraintParam.λ = 0.0f; // ラグランジュ乗数をリセット
	// α~ = α / (Δt^2)
	// ここでは1 / (compliance * dt^2) で計算されることが多いため、XPBD論文を参考に正確な計算をしてください
	// 例えば、alpha = 1.0f / (compliance * dt * dt) とか
	m_constraintParam.α = flexibility;//flexibility / 2.0f; // 仮にflexibilityをそのまま使う // 変えた　元々　flexiblity
}

float CollisionConstraint::EvaluateConstraint() const
{
	using namespace SimpleMath;


	float distance = m_pParticle->GetXi().Dot(m_collisionNormal) + m_planeDistance;

	// 違反度は正の数ののため符号反転
	return -distance;
}

float CollisionConstraint::ComputeLambdaCorrection(float dt, float C) const
{
	// 符号が反転されているので戻す
	C = -C;

	// PBDのΔλ = -C / (wi * |∇C|^2 + α~i)
	 // XPBDでは |∇C|^2 は |n|^2 = 1.0f
	 // wi は逆質量
	float invMass = m_pParticle->GetInvMass();
	float gradientSquared = m_collisionNormal.LengthSquared();// m_planeNormal.LengthSquared(); 
	if (gradientSquared < 1e-6f) return 0.0f; // ゼロ除算防止

	// 柔らかさのスケーリング (時間で正規化)
	float αTilda = m_constraintParam.α / (dt * dt);

	return -C / (invMass * gradientSquared + αTilda);
}

DirectX::SimpleMath::Vector3 CollisionConstraint::ComputeDeltaX(float deltaLambda) const
{
	// Δx = Δλ * ∇C * wi
	// ∇C は制約の勾配、ここでは法線ベクトル
	// normal : 戻す方向(押し出し方向)
	return deltaLambda * m_collisionNormal * m_pParticle->GetInvMass();
}

void CollisionConstraint::ApplyPositionCorrection(float deltaLambda)
{
	DirectX::SimpleMath::Vector3 deltaX = ComputeDeltaX(deltaLambda); // ComputeDeltaXが既にinvMassを考慮したΔxを返すと仮定
	m_pParticle->SetXi(m_pParticle->GetXi() + deltaX  + m_collisionNormal * 0.000001f); // 法線ベクトルにマジックナンバー掛けてとりあえず補正

	m_pParticle->m_planeNormal = m_collisionNormal;
}