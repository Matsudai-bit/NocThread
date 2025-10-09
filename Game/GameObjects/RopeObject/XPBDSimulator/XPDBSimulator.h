/*****************************************************************//**
 * @file    XPBDSimulator.h
 * @brief   XPDBのシミュレーターに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>

#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"


// クラスの前方宣言 ===================================================
class ParticleObject;
class RopeObject;
class DistanceConstraint;
class IConstraint;			// 制約インターフェース
class ConstraintFactory;	// 制約生成インターフェース

class CollisionManager; // 衝突管理

// クラスの定義 ===============================================================
/**
 * @brief XPDBシミュレーター
 */
class XPBDSimulator
{
// 構造体の宣言
public:
	struct Parameter
	{
		float flexibility = 0.001f;
		int iterations = 10;
		DirectX::SimpleMath::Vector3 gravity = DirectX::SimpleMath::Vector3(0.0f, 9.80f, 0.0f);
	};




public:

	/**
	 * @brief 使用パーティクル
	 */
	struct Particle
	{
		ParticleObject* pP;
		SimParticle		simP;

		Particle()
			: pP{ nullptr }
			, simP{}
		{}
	};

// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	std::vector<Particle> m_particles;	///< パーティクル群

	Parameter m_parameter;	///< パラメータ

	// 制約 処理速度担保のため分けておく
	std::vector<std::unique_ptr<IConstraint>> m_staticConstraints;  ///< 静的制約群 
	std::vector<std::unique_ptr<IConstraint>> m_dynamicConstraints; ///< 動的制約群　（毎フレーム更新される）

	std::vector<std::unique_ptr<ConstraintFactory>> m_constraintFactories; ///< 制約生成群

	// **** テスト用 :　絶対置き換えること ******
	CollisionManager* m_pCollisionManager;

	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	XPBDSimulator();

	// デストラクタ
	~XPBDSimulator();


// 操作
public:
	// 初期化処理
	void Initialize(Parameter parameter, RopeObject* pRopeObject);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();

	// シュミレーション
	void Simulate(float elapsedTime);

	// リセットする
	void Reset();

// 取得/設定
public:

	void SetCollisionManager(CollisionManager* pCollisionManager) { m_pCollisionManager = pCollisionManager; }

	// 制約の設定
	void SetConstraint(std::vector<std::unique_ptr<ConstraintFactory>>* constraintFactories);


// 内部実装
private:

	// 各パーティクルの予測位置（xi）を慣性に基づいて計算する
	void PredictNextPositions(float elapsedTime);

	// 各ロープセグメントの制約パラメータ（λ・α）を初期化する
	void ResetConstraintParameters();

	// 制約の生成
	void GenerateConstraints();

	// 各制約に対してXPBDの反復計算を行い、パーティクルの予測位置（xi）を調整する
	void IterateConstraints(float elapsedTime);

	// 予測位置と現在位置から速度を更新し、パーティクルの状態を確定させる
	void FinalizeVelocitiesAndPositions(float elapsedTime);



};
