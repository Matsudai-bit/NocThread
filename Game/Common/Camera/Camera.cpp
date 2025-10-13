/*****************************************************************//**
 * @file    Camera.h
 * @brief   カメラに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/02
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Camera.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Camera::Camera()
	: m_target	{ SimpleMath::Vector3(0.0f, 0.0f, 0.0f) }
	, m_eye		{ SimpleMath::Vector3(0.0f, 0.0f, 1.0f) }
{

}



/**
 * @brief デストラクタ
 */
Camera::~Camera()
{

}

/**
 * @brief 視点の設定
 * 
 * @param[in] eye視点
 */
void Camera::SetEye(const DirectX::SimpleMath::Vector3& eye)
{
	if (eye == m_target) return;
	m_eye = eye;
}

/**
 * @brief 注視点の設定
 * 
 * @param[in] target 注視店
 */
void Camera::SetTarget(const DirectX::SimpleMath::Vector3& target)
{
	if (target == m_eye) return;

	m_target = target;
}

void Camera::SetUp(const DirectX::SimpleMath::Vector3& up)
{
	m_up = up;
}

/**
 * @brief ビュー行列の取得
 * 
 * @return ビュー行列
 */
SimpleMath::Matrix Camera::GetView() const
{
	return SimpleMath::Matrix::CreateLookAt(m_eye, m_target, m_up);
}

/**
 * @brief ビュー座標の取得
 * 
 * @return ビュー座標
 */
DirectX::SimpleMath::Vector3 Camera::GetEye() const
{
	return m_eye;
}

DirectX::SimpleMath::Vector3 Camera::GetTarget() const
{
	return m_target;
}

/**
 * @brief 上ベクトルの取得
 * 
 * @return 上ベクトル
 */
DirectX::SimpleMath::Vector3 Camera::GetUp() const
{
	return m_up;
}



