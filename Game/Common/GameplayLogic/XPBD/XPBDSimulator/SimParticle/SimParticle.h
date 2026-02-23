/*****************************************************************//**
 * @file    SimParticle.h
 * @brief   シミュレーションで使われるパーティクルに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/14
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief シミュレーションで使われるパーティクル
 */
class SimParticle
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	bool m_isFixed;							///< 固定点かどうか
	DirectX::SimpleMath::Vector3 m_x;		///< 現在の座標
	DirectX::SimpleMath::Vector3 m_xi;		///< 予測座標 (演算用)
	DirectX::SimpleMath::Vector3 m_v;		///< 速度
	float m_mass;							///< 質量
	float m_w;								///< 逆質量(1/m)
	DirectX::SimpleMath::Vector3 initialPosition;	// 初期座標

public:
	DirectX::SimpleMath::Vector3 m_planeNormal;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SimParticle();

	// デストラクタ
	~SimParticle();


// 取得/設定
public:

	// 固定点かどうかの
	bool IsFixed() const { return m_isFixed; }
	void SetFixed(bool fixed) { m_isFixed = fixed; }

	// 現在の座標
	DirectX::SimpleMath::Vector3 GetX() const { return m_x; }
	void SetX(const DirectX::SimpleMath::Vector3& x) { m_x = x; }

	// 予測座標 (演算用)
	DirectX::SimpleMath::Vector3 GetXi() const { return m_xi; }
	void SetXi(const DirectX::SimpleMath::Vector3& xi) { m_xi = xi; }

	// 速度
	DirectX::SimpleMath::Vector3 GetV() const { return m_v; }
	void SetV(const DirectX::SimpleMath::Vector3& v) { m_v = v; }

	// 質量
	float GetMass() const { return m_mass; }
	void SetMass(float mass) {
		m_mass = mass;
		m_w = (mass != 0.0f) ? 1.0f / mass : 0.0f; // 質量が0のとき逆質量を0に
	}

	// 逆質量
	float GetInvMass() const { return m_w; }
	void SetInvMass(float w) { m_w = w; }

	// 初期座標
	DirectX::SimpleMath::Vector3 GetInitialPosition() const { return initialPosition; }
	void SetInitialPosition(const DirectX::SimpleMath::Vector3& pos) { initialPosition = pos; }
// 内部実装
private:


};
