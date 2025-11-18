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


	// --- カメラ設定関連 ---
	static constexpr float CAMERA_FOV_DEGREES = 45.0f;	///< 射影行列の視野角 (度)
	static constexpr float CAMERA_NEAR_CLIP = 0.1f;		///< 射影行列のニアクリップ距離
	static constexpr float CAMERA_FAR_CLIP = 450.0f;	///< 射影行列のファークリップ距離
// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector3 m_eye;			///< 視点
	DirectX::SimpleMath::Vector3 m_target;		///< 注視点
	DirectX::SimpleMath::Vector3 m_up;			///< カメラの上方向
	DirectX::SimpleMath::Matrix	 m_projection;	///< 射影行列
	DirectX::SimpleMath::Matrix	 m_view;		///< ビュー行列

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
	DirectX::SimpleMath::Matrix GetViewMatrix() const;

	// ビュー座標の取得
	DirectX::SimpleMath::Vector3 GetEye() const;
	DirectX::SimpleMath::Vector3 GetTarget() const;
	// 上ベクトルの取得
	DirectX::SimpleMath::Vector3 GetUp() const;

	// 射影行列の取得
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;

	// ビュー行列の算出
	void CalcViewMatrix();


// 内部実装
private:


};
