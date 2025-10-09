/*****************************************************************//**
 * @file    MovableObject.h
 * @brief   移動可能オブジェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/03/22
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/GameObjects/Common/GameObject.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 移動可能オブジェクト
 */
class MovableObject 
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector3 m_velocity; ///< 速度


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	MovableObject();

	// デストラクタ
	~MovableObject() = default;


// 操作
public:

	// 速度に力を加える
	void AddForceToVelocity(const DirectX::SimpleMath::Vector3& velocity);
	void AddForceToVelocityX(const float& vX);
	void AddForceToVelocityY(const float& vY);
	void AddForceToVelocityZ(const float& vZ);

	// 速度の初期化
	void ResetVelocity();
	void ResetVelocityX();
	void ResetVelocityY();
	void ResetVelocityZ();



	// 速度の取得
	DirectX::SimpleMath::Vector3 GetVelocity() const;

	// 速度の設定
	void SetVelocity(DirectX::SimpleMath::Vector3 velocity);

// 内部実装
private:


};
