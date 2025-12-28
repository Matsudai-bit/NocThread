#pragma once

#include "Game/GameObjects/Common/MovableObject/MovableObject.h"

// 操舵行動クラスを定義する
class SteeringBehavior
{
public:
	// 行動
	enum BEHAVIOR_TYPE
	{
		NONE = 0x0000, // なし
		ARRIVE = 0x0001, // 到着行動
		PURSUIT = 0x0002, // 追跡行動(2025/06/19)
	};
	// 「追跡」行動(2025/06/19)
	void PursuitOn() { m_flags |= BEHAVIOR_TYPE::PURSUIT; }
	void PursuitOff() { if (On(BEHAVIOR_TYPE::PURSUIT)) m_flags ^= BEHAVIOR_TYPE::PURSUIT; }
	bool IsPursuitOn() { return On(BEHAVIOR_TYPE::PURSUIT); }
	// 減速度
	enum class DECELERATION
	{
		SLOW = 3,
		NORMAL = 2,
		FAST = 1
	};

	// 指定行動中か調べる
	bool On(BEHAVIOR_TYPE behaviorType) const { return (m_flags & behaviorType) == behaviorType; }
	// 「到着」行動
	void ArriveOn() { m_flags |= BEHAVIOR_TYPE::ARRIVE; }
	void ArriveOff() { if (On(BEHAVIOR_TYPE::ARRIVE)) m_flags ^= BEHAVIOR_TYPE::ARRIVE; }
	bool IsArriveOn() const { return On(BEHAVIOR_TYPE::ARRIVE); }

	// ターゲットの位置を取得する
	DirectX::SimpleMath::Vector3 GetTargetPosition() const { return m_targetPosition; };
	// ターゲットの位置を設定する
	void SetTargetPosition(const DirectX::SimpleMath::Vector3& targetPosition) { m_targetPosition = targetPosition; 	}
	// 進行方向の成分を取得する
	float GetForwardComponent() const { return m_owner->GetTransform()->GetForward().Dot(m_steeringForce); }
	// 進行方向の横力の成分を取得する
	float GetSideComponent() const { return m_owner->GetTransform()->GetRight().Dot(m_steeringForce); }
	// 操舵力を取得する
	DirectX::SimpleMath::Vector3 GetForce() const { return m_steeringForce; }

	void SetOwner(MovableObject* owner) { m_owner = owner; };

public :
	// コンストラクタ
	SteeringBehavior(MovableObject* owner, const float& maxSpeed, const float& maxForce);
	// 操舵力を蓄積する
	bool AccumulateForce(DirectX::SimpleMath::Vector3& sf, const DirectX::SimpleMath::Vector3& forceToAdd);
	// 操舵力を計算する
	DirectX::SimpleMath::Vector3 Calculate();
	// 到着行動
	DirectX::SimpleMath::Vector3 Arrive(const DirectX::SimpleMath::Vector3& target, DECELERATION deceleration);



private:
	// サッカー選手
	MovableObject* m_owner;
	// ターゲット位置
	DirectX::SimpleMath::Vector3 m_targetPosition;

	// 操舵力
	DirectX::SimpleMath::Vector3 m_steeringForce;
	// ターゲットの位置(ボールの位置又は予測したボールの位置)
	DirectX::SimpleMath::Vector3 m_soccerBallTarget;
	// ビットフラグ(BEHAVIOR_TYPE)を立てることによって現在の行動を表す
	int m_flags;

	float m_maxSpeed;
	float m_steeringMaxForce;
};