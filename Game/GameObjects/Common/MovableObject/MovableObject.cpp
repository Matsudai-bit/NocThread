/*****************************************************************//**
 * @file    MovableObject.h
 * @brief   移動可能オブジェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/03/22
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "MovableObject.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
MovableObject::MovableObject()
	: m_velocity{0.0f, 0.0f, 0.0f}
{

}



/**
 * @brief 速度に力を加える
 *
 * @param[in] velocity 速度
 */
void MovableObject::AddForceToVelocity(const SimpleMath::Vector3& velocity)
{
	m_velocity += velocity;
}

void MovableObject::AddForceToVelocityX(const float& vX)
{
	AddForceToVelocity(SimpleMath::Vector3{ vX, 0.0f, 0.0f});
}

void MovableObject::AddForceToVelocityY(const float& vY)
{
	AddForceToVelocity(SimpleMath::Vector3{ 0.0f, vY, 0.0f });
}

void MovableObject::AddForceToVelocityZ(const float& vZ)
{
	AddForceToVelocity(SimpleMath::Vector3{ 0.0f, 0.0f, vZ });
}

/**
 * @brief 速度の初期化
 * 
 */
void MovableObject::ResetVelocity()
{
	m_velocity = SimpleMath::Vector3::Zero;
}

void MovableObject::ResetVelocityX()
{
	m_velocity.x = 0.0f;
}

void MovableObject::ResetVelocityY()
{
	m_velocity.y = 0.0f;

}

void MovableObject::ResetVelocityZ()
{
	m_velocity.z = 0.0f;
}



/**
 * @brief 速度の取得
 *
 * @return 速度
 */
SimpleMath::Vector3 MovableObject::GetVelocity() const
{
	return m_velocity;
}

/**
 * @brief 速度の設定
 * 
 * @param[in] velocity 速度
 */
void MovableObject::SetVelocity(DirectX::SimpleMath::Vector3 velocity)
{
	m_velocity = velocity;
}
