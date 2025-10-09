/*****************************************************************//**
 * @file    Camera.h
 * @brief   カメラに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/02
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief カメラ
 */
class Camera 
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector3 m_eye;		///< 視点
	DirectX::SimpleMath::Vector3 m_target;	///< 注視点
	DirectX::SimpleMath::Vector3 m_up;	///< 注視点

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Camera();

	// デストラクタ
	~Camera();


// 操作
public:

	// 視点の設定
	void SetEye(const DirectX::SimpleMath::Vector3& eye);
	// 注視点の設定
	void SetTarget(const DirectX::SimpleMath::Vector3& target);
	void SetUp(const DirectX::SimpleMath::Vector3& up);

	// ビュー行列の取得
	DirectX::SimpleMath::Matrix GetView() const;

	// ビュー座標の取得
	DirectX::SimpleMath::Vector3 GetEye() const;
	DirectX::SimpleMath::Vector3 GetTarget() const;



// 内部実装
private:


};
