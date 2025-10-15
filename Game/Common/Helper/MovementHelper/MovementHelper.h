/*****************************************************************//**
 * @file    MovementHelper.h
 * @brief   移動関連のヘルパー関数に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Library/MyLib/MathUtils/MathUtils.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 移動関連のヘルパー関数
 */
namespace MovementHelper
{
	/**
	 * @brief 速度を元に座標の算出
	 * 
	 * @param[in] deltaTime	経過時間
	 * @param[in] position		座標
	 * @param[in] velocity		速度
	 * 
	 * @return 座標
	 */
	inline DirectX::SimpleMath::Vector3 CalcPositionForVelocity(const float& deltaTime, const DirectX::SimpleMath::Vector3& position,const DirectX::SimpleMath::Vector3& velocity)
	{
		return position + velocity * deltaTime;
	}

	/**
	 * @brief 移動方向への回転結果を取得する
	 *
	 * @param[in] deltaTime		経過時間
	 * @param[in] currentRotate		現在の回転量
	 * @param[in] movementDirection	移動方向
	 * @param[in] forward			今の前方ベクトル
	 * @param[in] slerpT			回転率
	 *
	 * @return 移動方向への回転結果
	 */
	inline DirectX::SimpleMath::Quaternion RotateForMoveDirection(
		const float& deltaTime,
		const DirectX::SimpleMath::Quaternion& currentRotation,
		DirectX::SimpleMath::Vector3 currentForward,
		DirectX::SimpleMath::Vector3 movementDirection,
		const float& slerpT)
	{
		using namespace DirectX::SimpleMath;
		UNREFERENCED_PARAMETER(deltaTime);

		const float EPSILON = 0.0001f;

		// カレントとターゲットの方向をXZ平面に投影
		currentForward.y = 0.0f;

		movementDirection.y = 0.0f;

		if (movementDirection.LengthSquared() < EPSILON)
			return currentRotation;

		currentForward.Normalize();
		movementDirection.Normalize();


		// ほぼ同じならスキップ
		if ((movementDirection - currentForward).LengthSquared() < EPSILON)
			return currentRotation;

		// クォータニオンで向きを補間
		Quaternion targetRotation = Quaternion::CreateFromRotationMatrix(Matrix::CreateWorld(Vector3::Zero, movementDirection, Vector3::Up));

		Quaternion result = Quaternion::Slerp(currentRotation, targetRotation, slerpT); // 補間率を調整

		return result;
	}

	/**
	 * @brief 加速後の最大速度制限を適用した移動ベクトルを計算
	 * 
	 * 指定したmaxSpeedよりも大きくならないような速度を返す
	 *
	 * @param[in] currentVelocity        現在の水平方向の速度
	 * @param[in] requestedMoveDirection          プレイヤーの入力方向（正規化済み）
	 * @param[in] deltaTime            経過時間
	 * @param[in] accelerationRate       加速の強さ
	 * @param[in] decelerationRate       減速の強さ
	 * @param[in] maxSpeed               最大移動速度
	 * 
	 * @return DirectX::SimpleMath::Vector3 最終的な速度ベクトル
	 */
	inline DirectX::SimpleMath::Vector3 ClampedMovement(
		const DirectX::SimpleMath::Vector3& currentVelocity,
		const DirectX::SimpleMath::Vector3& requestedMoveDirection,
		const float& deltaTime,
		const float& accelerationRate,
		const float& decelerationRate,
		const float& maxSpeed)
	{
		using namespace DirectX::SimpleMath;

		
		Vector3 currentMovementDirection = currentVelocity;
		currentMovementDirection.Normalize();


		Vector3 acceleration = Vector3::Zero;
		// 入力方向と現在の速度方向の内積を計算
		if (MyLib::ApproxEqual(requestedMoveDirection.LengthSquared(), 0.0f) == false) // 入力がある場合
		{
			float dot = requestedMoveDirection.Dot(currentMovementDirection);

        
			// 内積の値を0.0fから1.0fの範囲に変換
			float dotNormalized = 1.0f - (dot + 1.0f) / 2.0f; // -1.0f -> 1.0f, 1.0f -> 0.0f

			// 加速と減速を内積の度合いに応じてブレンド
			float finalAccelRate =  accelerationRate -  ( accelerationRate - decelerationRate ) * dotNormalized;
        
			acceleration = requestedMoveDirection * finalAccelRate * deltaTime;


		}
		else
		{
			return Vector3::Zero;
		}

		

		// 新しい速度を計算 (v = v0 + a * t)
		Vector3 newVelocity = acceleration;

		Vector3 newDir = newVelocity;
		newDir.Normalize();
	
		// 最大速度を超えないように制限
		if ((newVelocity + currentVelocity).LengthSquared() * (newDir.Dot(currentMovementDirection) + 1.0f) / 2.0f > maxSpeed * maxSpeed)
		{
			newVelocity.Normalize();
			newVelocity *= std::max(maxSpeed - currentVelocity.Length(), 0.0f);
		}
		


		return newVelocity ;

	}
};
