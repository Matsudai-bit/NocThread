/*****************************************************************//**
 * @file    SteeringConstraint.h
 * @brief   衝突制約に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/07/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SteeringConstraint.h"

// ライブラリ関連
#include "Library\MyLib\MathUtils\MathUtils.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Graphics/Camera/Camera.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
SteeringConstraint::SteeringConstraint(const CommonResources* pCommonResources, SimParticle* pParticle, const DirectX::SimpleMath::Vector3& targetPosition)
	: m_pParticle{ pParticle }
	, m_constraintParam{}
	, m_targetPosition{ targetPosition }
	, m_pCommonResources{ pCommonResources }
{
	m_steeringBehavior = std::make_unique<SteeringBehavior>(18.0f, 0.01f);
}



/**
 * @brief デストラクタ
 */
SteeringConstraint::~SteeringConstraint()
{

}

/**
 * @brief パラメータをリセットする
 *
 * @param[in] flexibility　柔軟性
 */
void SteeringConstraint::ResetConstraintParam(float flexibility)
{
	m_constraintParam.λ = 0.0f; // ラグランジュ乗数をリセット
	// α~ = α / (Δt^2)
	// ここでは1 / (compliance * dt^2) で計算されることが多いため、XPBD論文を参考に正確な計算をしてください
	// 例えば、alpha = 1.0f / (compliance * dt * dt) とか
	m_constraintParam.α = flexibility;//flexibility / 2.0f; // 仮にflexibilityをそのまま使う // 変えた　元々　flexiblity
}

float SteeringConstraint::EvaluateConstraint() const
{
	using namespace SimpleMath;
	// C = |xi - target|
	Vector3 diff = m_pParticle->GetXi() - m_targetPosition;
	return diff.Length();
}

float SteeringConstraint::ComputeLambdaCorrection(float dt, float C) const
{

	auto vector = m_targetPosition - m_pParticle->GetXi();
	auto distance = vector.Length();

	// 柔らかさのスケーリング (時間で正規化)
	auto αTilda = m_constraintParam.α / (dt * dt);

	if (distance < 1e-6f) return 0.0f;

	/// 単位方向ベクトル (制約勾配)
	auto deltaC = vector / distance; // ΔC

	// 合計の逆質量
	float invM = m_pParticle->GetInvMass() ;

	// 有効質量 + 柔軟性による安定化項
	return (C - αTilda * m_constraintParam.λ) / (deltaC.Dot(invM * deltaC) + αTilda);
}

DirectX::SimpleMath::Vector3 SteeringConstraint::ComputeDeltaX(float deltaLambda) const
{
	using namespace SimpleMath;

	Vector3 diff = m_targetPosition - m_pParticle->GetXi() ;
	float dist = diff.Length();

	if (dist < 1e-6f) return Vector3::Zero;

	// 勾配 ∇C = (xi - target) / |xi - target|
	Vector3 gradient = diff / dist;

	// Δx = Δλ * ∇C
	return deltaLambda * gradient;
}

void SteeringConstraint::ApplyPositionCorrection(float deltaLambda)
{
	// 1. XPBDとしての補正量を算出
	auto deltaX = ComputeDeltaX(deltaLambda);

	// 2. ステアリング行動で補正をフィルタリング（滑らかにする）
	m_steeringBehavior->SetTargetPosition(m_targetPosition);
	auto steeringForce = m_steeringBehavior->Calculate(m_pParticle->GetXi(), m_pParticle->GetInvMass() * deltaX);

	// 3. 位置を更新
	m_pParticle->SetXi(m_pParticle->GetXi() + steeringForce);

}