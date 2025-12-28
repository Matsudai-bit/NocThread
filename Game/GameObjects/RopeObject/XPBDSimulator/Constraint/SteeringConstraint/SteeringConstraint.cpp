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
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

#include "Library\MyLib\MathUtils\MathUtils.h"
#include "Game/Common/Collision/Collision.h"
#include "Game/Common/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Camera/Camera.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
SteeringConstraint::SteeringConstraint(SimParticle* pParticle, const DirectX::SimpleMath::Vector3& targetPosition)
	: m_pParticle{ pParticle }
	, m_constraintParam{}
	, m_targetPosition{ targetPosition }
{

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
	return 1.0f;
	float distanceSqr = Vector3::DistanceSquared(m_targetPosition, m_pParticle->GetX());
	if (MyLib::ApproxEqual(distanceSqr, 0.0f))
	{
		return 0.0f;
	}

	return 1.0f - (1.0f / distanceSqr);
}

float SteeringConstraint::ComputeLambdaCorrection(float dt, float C) const
{

	return dt;
}

DirectX::SimpleMath::Vector3 SteeringConstraint::ComputeDeltaX(float deltaLambda) const
{
	using namespace SimpleMath;
	auto kb = Keyboard::Get().GetState();

	// 入力に応じた水平ベクトル
	Vector3 inputVector;
	if (kb.A)	inputVector += Vector3::Left;
	if (kb.D)	inputVector += Vector3::Right;
	if (kb.W)		inputVector += Vector3::Up;
	if (kb.S)	inputVector += Vector3::Down;

	if (MyLib::ApproxEqual(inputVector.LengthSquared(), 0.0f))
	{
		return Vector3::Zero;
	}

	// 現在のカメラの方向た方向を求める(Y軸だけ回転させる)
	auto camera = MainCamera::GetInstance()->GetCamera();
	auto cameraForward = camera->GetTarget() - camera->GetEye();
	
	
	cameraForward.y = 0.0f;
	cameraForward.Normalize();

	// カメラの「右方向」を取得（カメラの向き × Y軸）
	SimpleMath::Vector3 cameraRight = cameraForward.Cross(SimpleMath::Vector3::Up);
	cameraRight.Normalize();

	// ローカル入力をカメラ空間に変換（X:右、Z:前）
	// Z方向が正面同士の時に-の乗算になり結果敵に反対方向に向いてしまうため-で補正
	SimpleMath::Vector3 worldMovementDir = -inputVector.z * cameraForward + inputVector.x * cameraRight;

	worldMovementDir.Normalize();


	return worldMovementDir * 0.5f * deltaLambda;
}

void SteeringConstraint::ApplyPositionCorrection(float deltaLambda)
{
	auto deltaX = ComputeDeltaX(deltaLambda);
	m_pParticle->SetXi(m_pParticle->GetXi() + deltaX);

}