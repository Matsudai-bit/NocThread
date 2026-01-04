#include "pch.h"
#include "SteeringBehavior.h"

#include "SteeringParameters.h"
// コンストラクタ
SteeringBehavior::SteeringBehavior(const float& maxSpeed, const float& maxForce)
	:
	m_targetPosition{},										// ターゲット位置
	m_steeringForce{},										// 操舵力
	m_soccerBallTarget{},									// ターゲットの位置(ボールの位置又は予測したボールの位置)
	m_flags{}		,												// ビットフラグ
	m_maxSpeed{ maxSpeed },
	m_steeringMaxForce{maxForce}
	

{}

// 操舵力を蓄積する
bool SteeringBehavior::AccumulateForce(DirectX::SimpleMath::Vector3& steeringForce, const DirectX::SimpleMath::Vector3& forceToAdd)
{
	float magnitudeRemaining = m_steeringMaxForce - steeringForce.Length();

	// 加えられる操舵力に余裕がないのでfalseを返す
	if (magnitudeRemaining <= 0.0f)
		return false;

	// 加える操舵力の大きさ
	float magnitudeToAdd = forceToAdd.Length();
	// 加える操舵力が加えられる操舵力を超えていれば調整する  
	if (magnitudeToAdd > magnitudeRemaining) { magnitudeToAdd = magnitudeRemaining; }
	// 操舵力を加える
	DirectX::SimpleMath::Vector3 forceToAddN = forceToAdd;
	// 正規化する
	forceToAddN.Normalize();
	// 操舵力を加える
	steeringForce += forceToAddN * magnitudeToAdd;
	// 加えられる操舵力に余裕があるのでtrueを返す
	return true;
}

// 操舵力を計算する
DirectX::SimpleMath::Vector3 SteeringBehavior::Calculate(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity)
{
	DirectX::SimpleMath::Vector3 force = DirectX::SimpleMath::Vector3::Zero;;
	// 操舵力をリセットする
	m_steeringForce = DirectX::SimpleMath::Vector3::Zero;
	// 各行動による操舵力を加算する
	// 「到着」行動
/*	if (On(BEHAVIOR_TYPE::ARRIVE))
	{
	*/	// 到着行動を行う
		force += Arrive(position, velocity, m_targetPosition, DECELERATION::FAST) * SteeringParameters::ARRIVE_WEIGHT;
		if (!AccumulateForce(m_steeringForce, force))
			return m_steeringForce;
//	}
	
	// 操舵力を返す
	return m_steeringForce;
}

// 「到着」行動
DirectX::SimpleMath::Vector3 SteeringBehavior::Arrive(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Vector3& velocity,
	const DirectX::SimpleMath::Vector3& targetPosition,
	DECELERATION deceleration)
{

	// ターゲットに向かうためのベクトルを計算する
	DirectX::SimpleMath::Vector3 toTarget = targetPosition - position;
	// 目標位置までの距離を計算する
	float distance = toTarget.Length();
	// 目的位置までの距離が0より大きい場合
	if (distance > 0.0f)
	{
		// 減速率(減速の微調整に使う)
		const float decelerationTweaker = 3.0f;
		// 指定された望ましい減速で目標に到達するのに必要な速度を計算する
		float speed = distance / ((float)deceleration * decelerationTweaker);
		// 速度が最大移動速度を超えないようにする
		speed = std::min(speed, m_maxSpeed);

		// 「到着」行動の様な処理を行う
		DirectX::SimpleMath::Vector3 desiredVelocity = toTarget * speed / distance;
		// 操舵力を返す
		return (desiredVelocity - velocity);
	}
	// 操舵力ゼロを返す
	return DirectX::SimpleMath::Vector3::Zero;
}

