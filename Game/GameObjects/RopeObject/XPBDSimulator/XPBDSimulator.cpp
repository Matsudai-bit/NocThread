/*****************************************************************//**
 * @file    XPBDSimulator.h
 * @brief   XPDBのシミュレーターに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "XPBDSimulator.h"

#include "Game/GameObjects/RopeObject/RopeObject.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/DistanceConstraint/DistanceConstraint.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/IConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraint.h"

#include "GeometricPrimitive.h"
#include "Library/MyLib\MathUtils\MathUtils.h"

#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/Common/Collision/Collision.h"

// 制約生成
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraintFactory.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/DistanceConstraint/DistanceConstraintFactory.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
XPBDSimulator::XPBDSimulator()
	: m_pCollisionManager{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
XPBDSimulator::~XPBDSimulator()
{

}



/**
 * @brief 初期化処理
 * 
 * @param[in] parameter		パラメータ
 * @param[in] pRopeObject	ロープオブジェクト
 */
void XPBDSimulator::Initialize(Parameter parameter, RopeObject* pRopeObject)
{
	// 初期化
	Reset();

	m_parameter = parameter;


	// **** パーティクル配列の初期化 ****
	const auto pRopeParticles = pRopeObject->GetParticles();	// 処理用変数

	size_t particleNum = pRopeParticles->size();

	if (particleNum <= 0) particleNum = 2;

	// パーティクルの配列の領域を確保する
	m_particles.resize(particleNum);


	//// ロープ同士の管理配列の領域を確保する
	//m_ropeSegments.resize(particleNum - 1);

	// パーティクル・ロープ同士管理の登録
	for (size_t i = 0; i < particleNum; i++)
	{
		m_particles[i].pP = (*pRopeParticles)[i];

		// 処理用変数
		ParticleObject* pParticleObj =  m_particles[i].pP;
		SimParticle*	pSimParticle = &m_particles[i].simP;

		/**** シミュレーションパーティクルの設定 ***/
		// 固定するかどうか
		pSimParticle->SetFixed(i == 0);
		// 初期座標の設定
		pSimParticle->SetInitialPosition(pParticleObj->GetPosition());

		// 現在座標の設定
		pSimParticle->SetX(pSimParticle->GetInitialPosition());
		// 質量の設定
		pSimParticle->SetMass(pParticleObj->GetMass());
		// 逆質量の設定
		pSimParticle->SetInvMass(1.0f / pSimParticle->GetMass());
		// 速度の設定
		pSimParticle->SetV(pParticleObj->GetVelocity());
		
		


	}

	// 標準として距離制約を登録する
	m_constraintFactories.emplace_back(std::make_unique<DistanceConstraintFactory>());
	
	// 静的な制約を初期化
	m_staticConstraints.clear();

	for (auto& factory : m_constraintFactories)
	{
		// 動的な場合まだ作成しない
		if (factory->IsDynamic()) continue; 

		// 生成した制約の格納配列
		std::vector<std::unique_ptr<IConstraint>> creationConstraints;

		// 生成
		creationConstraints = factory->CreateConstraint(&m_particles);

		// 生成した制約を登録する
		for (int i = 0; i < creationConstraints.size(); i++)
		{
			m_staticConstraints.push_back(std::move(creationConstraints[i]));
		}

	}
	
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void XPBDSimulator::Update(float deltaTime)
{

	// **** 固定点以外の位置をデータに反映 ****
	for (auto& particle : m_particles)
	{
		if (!particle.simP.IsFixed())  continue; 
		// 反映
		particle.simP.SetX(particle.pP->GetPosition());
	}

	Simulate(deltaTime);

	// **** シミュレーション結果の位置をデータに反映 ****
	for (auto& particle : m_particles)
	{
		// 反映
		particle.pP->SetPosition(particle.simP.GetX());

		// 追加
		particle.pP->SetVelocity(particle.simP.GetV());
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void XPBDSimulator::Draw()
{

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void XPBDSimulator::Finalize()
{

}

/**
 * @brief シュミレーション
 * 
 * @param[in] deltaTime　経過時間
 */
void XPBDSimulator::Simulate(float deltaTime)
{
	// 予測位置の算出
	PredictNextPositions(deltaTime);

	// 制約の生成
	GenerateConstraints();

	// 制約の初期化
	ResetConstraintParameters();

	// 各制約に対してXPBDの反復計算を行い、パーティクルの予測位置（xi）を調整する
	IterateConstraints(deltaTime);

	// 予測位置と現在位置から速度を更新
	FinalizeVelocitiesAndPositions(deltaTime);
}

/**
 * @brief リセットする
 * 
 */
void XPBDSimulator::Reset()
{
	m_particles.clear();
}

/**
 * @brief 制約の設定
 * 
 * @param[in] constraintFactories 設定する制約
 */
void XPBDSimulator::SetConstraint(std::vector<std::unique_ptr<ConstraintFactory>>* constraintFactories)
{
	for (int i = 0; i < constraintFactories->size(); i++)
	{
		m_constraintFactories.push_back(std::move((*constraintFactories)[i]));
	}
}

/**
 * @brief 各パーティクルの予測位置（xi）を慣性に基づいて計算する
 *
 * 現在位置と速度を使って、次のフレームでの予測位置を求めます。
 * 外力（重力など）はこの時点では加味しません。
 *
 * @param[in] deltaTime 経過時間（Δt）
 */
void XPBDSimulator::PredictNextPositions(float deltaTime)
{

	// x~ = x + Δt v + Δt^2 M^-1 f_ext(x)
	// **** 次の地点の予測 ****
	for (auto& particle : m_particles)
	{
		// 処理用変数
		auto* simP = &particle.simP;

		// 次の地点
		auto xTilda = simP->GetX() + deltaTime * simP->GetV();
		// 座標の更新
		simP->SetXi(xTilda);


	}
}

/**
 * @brief 各ロープセグメントの制約パラメータ（λ・α）を初期化する
 *
 * XPBDにおいては、毎フレーム制約のラグランジュ乗数（λ）をリセットし、
 * 柔軟性（α）を更新してからソルバを開始する必要があります。
 */
void XPBDSimulator::ResetConstraintParameters()
{
	// 制約の初期化
	for (auto& constraint : m_staticConstraints)
	{
		constraint->ResetConstraintParam(m_parameter.flexibility);
	}
}

/**
 * @brief 制約の生成
 * 
 * パーティクルの距離制約以外の制約の生成
 * 
 */
void XPBDSimulator::GenerateConstraints()
{
	// 動的制約のクリア
	m_dynamicConstraints.clear();

	
	// 制約の生成
	for (auto& constraintFactory : m_constraintFactories)
	{
		// 生成した制約の格納配列
		std::vector<std::unique_ptr<IConstraint>> creationConstraints;

		// 生成
		creationConstraints = constraintFactory->CreateConstraint(&m_particles);

		// 生成した制約を登録する
		for (int i = 0; i < creationConstraints.size(); i++)
		{
			m_dynamicConstraints.push_back(std::move(creationConstraints[i]));
		}

		
	}

}

/**
 * @brief 各制約に対してXPBDの反復計算を行い、パーティクルの予測位置（xi）を調整する
 *
 * 各ロープセグメントの距離制約に違反している分を補正するために、
 * Δλ（補正係数）とΔx（補正ベクトル）を計算し、
 * パーティクルの予測位置に反映していきます。
 * 指定されたイテレーション回数だけ反復して、安定した解に近づけます。
 *
 * @param[in] deltaTime 経過時間（Δt）
 */
void XPBDSimulator::IterateConstraints(float deltaTime)
{
	using namespace SimpleMath;

	// 制約を集約する
	std::vector<IConstraint*> constraints;
	//constraints.resize(m_constraints.size() + m_ropeSegments.size());
	// コピーを取って使う
	//constraints.reserve(m_ropeSegments.size() + m_constraints.size());

	for (auto& con : m_staticConstraints)
	{
		constraints.push_back(con.get());
	}
	for (auto& con : m_dynamicConstraints)
	{
		constraints.push_back(con.get());
	}

	// イテレーション数分まわす
	   // iterations 回の反復で制約を解決し、xi をどんどん調整。
	//m_parameter.iterations = 1;
	int i = 0;
	while (i < m_parameter.iterations)
	{
		for (auto& constraint : constraints) // m_ropeSegments の代わりに m_constraints をループ
		{
			// 固定されているパーティクルに関わる制約はスキップ (制約内でチェック済みなら不要)
			// SimParticle内でFixedなパーティクルはSetXiなどをスキップするようにしても良い

			// 制約の取得
			ConstraintParam cParam = constraint->GetConstraintParam();

			// 制約違反の評価
			float C = constraint->EvaluateConstraint();

			// 貫通していない（または規定の範囲内）であればスキップ
			// (これは特に衝突制約で重要)
			if (C <= 0.0f) // 0以下であれば制約違反なしとみなす（貫通していない）
			{
				// ここで摩擦などを考慮する場合は別途処理が必要
				continue;
			}
	

			// Δλの計算
			float Δλ = constraint->ComputeLambdaCorrection(deltaTime, C);

			// 制約の更新 (ラグランジュ乗数の蓄積)
			cParam.λ = cParam.λ + Δλ;
			constraint->SetConstraintParam(cParam);

			// 位置の補正ベクトル Δx の計算

			// 各制約が自身に関わるパーティクルに補正を適用する
			constraint->ApplyPositionCorrection(Δλ); // 仮のメソッド
		}
		i = i + 1;
	}

	constraints.clear();
}

/**
 * @brief 予測位置と現在位置から速度を更新し、パーティクルの状態を確定させる
 *
 * 補正された予測位置（xi）と現在位置（x）の差分に基づいて速度を再計算し、
 * パーティクルの現在位置（x）を予測位置に更新します。
 * 外力（重力）もここで加味します。
 *
 * @param[in] deltaTime 経過時間（Δt）
 */
void XPBDSimulator::FinalizeVelocitiesAndPositions(float deltaTime)
{
	for (auto& particle : m_particles)
	{
		// 処理用変数
		auto* simP = &particle.simP;

		if (simP->IsFixed())  continue;

		// 速度の更新
		SimpleMath::Vector3 v_potential = ((simP->GetXi() - simP->GetX()) / deltaTime + m_parameter.gravity * deltaTime);
		simP->SetV(v_potential + m_parameter.gravity * deltaTime);

		particle.simP.m_planeNormal = SimpleMath::Vector3::Zero;
		// 座標の更新
		// 予測点を現在の座標とする
		simP->SetX(simP->GetXi());
	}
}
