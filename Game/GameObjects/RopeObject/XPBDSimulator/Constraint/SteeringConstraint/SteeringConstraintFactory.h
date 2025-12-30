/*****************************************************************//**
 * @file    SteeringConstraintFactory.h
 * @brief   操舵制約の生成器に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/ConstraintFactory.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/XPBDSimulator.h"

// クラスの前方宣言 ===================================================
class SimParticle; // シミュレーションのパーティクル

class ICollider; // コライダーインターフェース
class CollisionManager; // 衝突管理
class IConstraint;
class CommonResources;
// クラスの定義 ===============================================================
/**
 * @brief 操舵制約の生成器
 */
class SteeringConstraintFactory
	: public ConstraintFactoryBase
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	XPBDSimulator::Parameter m_paramater;

	const CommonResources* m_pCommonResources;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
	std::unique_ptr < DirectX::BasicEffect> m_basicEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SteeringConstraintFactory(const CommonResources* pCommonResources, XPBDSimulator::Parameter paramater);

	// デストラクタ
	~SteeringConstraintFactory();


// 操作
public:
	
	// 制約の作成
	std::vector<std::unique_ptr<IConstraint>> CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles) override;

// 取得/設定
public:

// 内部実装
private:


};
