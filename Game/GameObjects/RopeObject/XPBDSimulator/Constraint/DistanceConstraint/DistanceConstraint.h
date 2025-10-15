/*****************************************************************//**
 * @file    DistanceConstraint.h
 * @brief   距離制約に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/14
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/GameObjects/RopeObject/XPBDSimulator/XPBDSimulator.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/IConstraint.h"

// クラスの前方宣言 ===================================================
class SimParticle;	// シュミレータのパーティクル

// クラスの定義 ===============================================================
/**
 * @brief 距離制約
 */
class DistanceConstraint
	: public IConstraint
{
// 構造体の宣言 -------------------------------------------------
public:

	/**
	 * @brief 接続されるパーティクル
	 */
	struct  ConnectParticle
	{
		SimParticle* pP1;	///< パーティクル1
		SimParticle* pP2; ///< パーティクル2
	};



// クラス定数の宣言 -------------------------------------------------
public:

	ConstraintParam m_constraint;	///< 制約

	ConnectParticle m_connectParticle;				///< 接続されるパーティクル

// データメンバの宣言 -----------------------------------------------
private:

	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	DistanceConstraint(SimParticle* pP1, SimParticle* pP2);

	// コンストラクタ
	DistanceConstraint();

	// デストラクタ
	~DistanceConstraint();


// 操作
public:
	// 初期化処理
	void Initialize(SimParticle* pP1, SimParticle* pP2);

	// パラメータをリセットする
	void ResetConstraintParam(float flexibility) override;

	// 制約の現在の違反度(C)を計算する
	float EvaluateConstraint() const override;
	// ラグランジュ乗数の更新量 Δλ を計算する
	float ComputeLambdaCorrection(float dt, float C) const override;
	// 位置の補正ベクトル Δx を計算する
	DirectX::SimpleMath::Vector3 ComputeDeltaX(float deltaLambda) const override;

	// 位置補正の適用
	void ApplyPositionCorrection(float deltaLambda) override;


// 取得/設定
public:

	// 制約の取得
	const ConstraintParam& GetConstraintParam() const override { return m_constraint; }

	// 制約の設定
	void SetConstraintParam(const ConstraintParam& constraint) override  { m_constraint = constraint; }

	// 接続されるパーティクルの取得
	ConnectParticle GetConnectParticle()  { return m_connectParticle; }

	// 接続パーティクルの設定
	void SetConncectParticle(SimParticle* pP1, SimParticle* pP2) 
	{ 
		m_connectParticle.pP1 = pP1;
		m_connectParticle.pP2 = pP2; 
	};

	// 接続パーティクルの設定
	void SetConncectParticleXi(const DirectX::SimpleMath::Vector3& pP1, const DirectX::SimpleMath::Vector3& pP2)
	{
		m_connectParticle.pP1->SetXi(pP1);
		m_connectParticle.pP2->SetXi(pP2);
	};



// 内部実装
private:


};
