/*****************************************************************//**
 * @file    Transform.h
 * @brief   位置、回転、スケールを管理するクラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/11/20
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 位置、回転、スケールを管理するクラス
 */
class Transform
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector3	m_position;		///< 座標
	DirectX::SimpleMath::Quaternion m_rotation;		///< 回転
	DirectX::SimpleMath::Quaternion m_initialRotation;///< 初期回転　モデルを初期状態で傾け解く場合などに使用する
	DirectX::SimpleMath::Vector3	m_scale;		///< スケール

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Transform();

	// デストラクタ
	~Transform();


// 操作
public:
	
	// 正面ベクトルの取得
	DirectX::SimpleMath::Vector3 GetForward() const;

// 取得/設定
public:

	// 座標
	DirectX::SimpleMath::Vector3 GetPosition() const				{ return m_position; }
	void SetPosition(const DirectX::SimpleMath::Vector3& position)	{ m_position = position; }

	// 回転
	DirectX::SimpleMath::Quaternion GetRotation() const					{ return m_rotation; }
	void SetRotation(const DirectX::SimpleMath::Quaternion& rotation)	{ m_rotation = rotation; }

	// 初期回転
	DirectX::SimpleMath::Quaternion GetInitialRotation() const				{ return m_initialRotation;}
	void SetInitialRotation(const DirectX::SimpleMath::Quaternion& rotation){ m_initialRotation = rotation; }

	// スケール
	DirectX::SimpleMath::Vector3 GetScale() const					{ return m_scale; }
	void SetScale(const DirectX::SimpleMath::Vector3& scale)		{ m_scale = scale; }
	void SetScale(const float& scale)								{ m_scale = DirectX::SimpleMath::Vector3( scale, scale, scale); }

	
// 内部実装
private:


};
