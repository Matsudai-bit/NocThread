/*****************************************************************//*
 * @file    PhysicsHelper.h
 * @brief   物理関連のヘルパー関数に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 物理関連のヘルパー関数
 */
namespace PhysicsHelper
{
	/**
	 * @brief バウンドの解消
	 * 
	 * @param[in] overlap		押し出し
	 * @param[in] restitution	
	 * @param[in] velocity
	 * 
	 * @return バウンドを加えて速度
	 */
	inline DirectX::SimpleMath::Vector3 ResolveBounce(DirectX::SimpleMath::Vector3 overlap, const float& restitution, DirectX::SimpleMath::Vector3 velocity)
	{
		using namespace DirectX;

		SimpleMath::Vector3 overlapRaw = overlap;
		SimpleMath::Vector3 overlapDir = overlap;
		overlapDir.Normalize();

		float dot = velocity.Dot(overlapDir);

		// 速度の反転（バウンド成分）
		SimpleMath::Vector3 bounce = -overlapDir * dot * restitution;

		// 壁に沿った滑り成分
		SimpleMath::Vector3 slide = velocity - overlapDir * dot;

		// 最終的な速度
		return slide + bounce;
	}

	/**
	 * @brief 押し出し
	 * 
	 * @param[in] overlap　押し出し量
	 * @param[in] position 座標
	 * 
	 * @return 押し出し後の座標
	 */
	inline DirectX::SimpleMath::Vector3 PushOut(DirectX::SimpleMath::Vector3 overlap, DirectX::SimpleMath::Vector3 position)
	{

		return position + overlap;
	}

	/**
 * @brief 速度に空気抵抗を適用した後の速度を計算
 *
 * @param[in] currentVelocity    現在の速度
 * @param[in] elapsedTime        経過時間
 * @param[in] dragCoefficient    空気抵抗係数（減速の強さ）
 * 
 * @return DirectX::SimpleMath::Vector3 空気抵抗適用後の速度
 */
	inline DirectX::SimpleMath::Vector3 CalculateDragVelocity(
		const DirectX::SimpleMath::Vector3& currentVelocity,
		const float& elapsedTime,
		const float& dragCoefficient)
	{
		using namespace DirectX::SimpleMath;

		Vector3 velocity = currentVelocity;
		float speed = velocity.Length();

		if (speed > 0.01f)
		{
			// 速度の2乗に比例するドラッグフォースを計算
			// F_drag = -k * v^2
			Vector3 counterDir = -velocity;
			counterDir.Normalize();

			Vector3 dragForce = counterDir * (speed * speed) * dragCoefficient;

			// 加速度を適用して速度を更新 (v = v0 + at)
			velocity += dragForce * elapsedTime;

			// ゼロに近づきすぎた場合、ゼロにクランプする
			if (velocity.LengthSquared() < 0.0001f)
			{
				velocity = Vector3::Zero;
			}
		}

		return velocity;
	}


	/**
	 * @brief 速度に摩擦力を適用した後の速度を計算
	 *
	 * @param[in] currentVelocity    現在の速度
	 * @param[in] elapsedTime        経過時間
	 * @param[in] frictionCoefficient 摩擦係数
	 * @param[in] normalForce        垂直抗力（例: 重力や地面からの反発力）
	 * @return DirectX::SimpleMath::Vector3 摩擦適用後の速度
	 */
	inline DirectX::SimpleMath::Vector3 CalculateFrictionVelocity(
		const DirectX::SimpleMath::Vector3& currentVelocity,
		const float& elapsedTime,
		const float& frictionCoefficient,
		const float& normalForce)
	{
		using namespace DirectX::SimpleMath;

		Vector3 velocity = currentVelocity;
		float speed = velocity.Length();

		if (speed > 0.01f)
		{
			// 動摩擦力の大きさを計算
			// F_friction = -μk * N
			float frictionMagnitude = frictionCoefficient * normalForce;

			// 摩擦力によって減速する量を計算
			// 慣性の法則から、a = F/m。mを1と仮定して、a = F
			float deceleration = frictionMagnitude;

			// 速度がゼロに達するまでの時間を計算
			float timeToStop = speed / deceleration;

			if (elapsedTime > timeToStop)
			{
				// このフレームで完全に停止する場合
				velocity = Vector3::Zero;
			}
			else
			{
				// 速度の方向はそのままに、大きさを減らす
				auto dir = velocity;
				dir.Normalize();
				velocity -= dir * deceleration * elapsedTime;
			}
		}

		return velocity;
	}
};
