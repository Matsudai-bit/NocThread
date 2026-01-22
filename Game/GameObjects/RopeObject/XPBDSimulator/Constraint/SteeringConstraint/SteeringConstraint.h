/*****************************************************************//**
 * @file    SteeringConstraint.h
 * @brief   衝突制約に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/09
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/Behaviour/SteeringBehavior/SteeringBehavior.h"

// ユーティリティ関連
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/IConstraint.h"

// クラスの前方宣言 ===================================================
class SimParticle; // シミュレーションのパーティクル

class ICollider; // コライダーインターフェース
class CommonResources;

// クラスの定義 ===============================================================
/**
 * @brief 衝突制約
 */
class SteeringConstraint
	: public IConstraint
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	ConstraintParam m_constraintParam;

	SimParticle* m_pParticle;

	DirectX::SimpleMath::Vector3 m_targetPosition;

	std::unique_ptr<SteeringBehavior> m_steeringBehavior;

	const CommonResources* m_pCommonResources;

public:




// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SteeringConstraint(const CommonResources* pCommonResources, SimParticle* pParticle,const DirectX::SimpleMath::Vector3& targetPosition);

	// デストラクタ
	~SteeringConstraint();


// 操作
public:
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
	const ConstraintParam& GetConstraintParam() const override { return m_constraintParam; }

	// 制約の設定
	void SetConstraintParam(const ConstraintParam& constraint) override { m_constraintParam = constraint; }
// 内部実装
private:


};
