/*****************************************************************//**
 * @file    Transform.h
 * @brief   位置、回転、スケールを管理するクラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/11/20
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Transform.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Transform::Transform()
{

}



/**
 * @brief デストラクタ
 */
Transform::~Transform()
{

}

/**
 * @brief 正面ベクトルの取得
 * 
 * @return 正面ベクトル
 */
DirectX::SimpleMath::Vector3 Transform::GetForward() const
{
	// ワールド正面ベクトル
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Forward;
	// ローカルに修正する
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateFromQuaternion(GetRotation());

	return SimpleMath::Vector3::TransformNormal(forward, rot);
}

/**
 * @brief 右ベクトルの取得
 * 
 * @return 右ベクトル
 */
DirectX::SimpleMath::Vector3 Transform::GetRight() const
{
	// 右に分解する
	SimpleMath::Vector3 right = SimpleMath::Vector3::Right;
	// ローカルに修正する
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateFromQuaternion(GetRotation());

	return SimpleMath::Vector3::TransformNormal(right, rot);
}



