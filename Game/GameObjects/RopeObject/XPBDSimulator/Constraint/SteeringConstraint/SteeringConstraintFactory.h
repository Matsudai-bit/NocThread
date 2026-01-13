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
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

#include "Game/Common/Input/InputSystem/InputSystem.h"
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"

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

	static constexpr float DUMPING_DURATION = 1.0f; // ワイヤーの慣性が減衰しきる時間
	static constexpr float AUTO_INERTIA_STRENGTH = 50.0f; // 自動でかかる慣性の強さ
	static constexpr float INPUT_INERTIA_STRENGTH = 130.0f; // 入力によりでかかる慣性の強さ

// データメンバの宣言 -----------------------------------------------
private:

	const CommonResources* m_pCommonResources;

	ElapsedTimeCounter m_elapsedTimeCounter;	///< 経過時間カウンター

	std::unique_ptr<InputSystem<InputActionType::PlyayerActionID>> m_playerInput; ///< プレイヤー入力

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SteeringConstraintFactory(const CommonResources* pCommonResources);

	// デストラクタ
	~SteeringConstraintFactory();


// 操作
public:
	// リセット
	void Reset() override;
	// 制約の作成
	std::vector<std::unique_ptr<IConstraint>> CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater) override;

// 取得/設定
public:

// 内部実装
private:

	// 入力に対するステアリング力を計算する
	DirectX::SimpleMath::Vector3 ComputeInputSteeringForce(const DirectX::SimpleMath::Vector3& cameraForward) const;

	// カメラが向く方向へ導くステアリング力を計算する
	DirectX::SimpleMath::Vector3 ComputeCameraDirectedSteeringForce(
		const DirectX::SimpleMath::Vector3& cameraForward,
		const DirectX::SimpleMath::Vector3& particleRootPosition,
		const SimParticle& particle,
		const float& inertiaDamping) const;

};
