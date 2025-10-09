/*****************************************************************//**
 * @file    XPBDSimulator.h
 * @brief   XPDBのシミュレーターに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "XPDBSimulator.h"

#include "Game/GameObjects/RopeObject/RopeObject.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/RopeSegment/RopeSegment.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/IConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraint.h"

#include "GeometricPrimitive.h"
#include "Library/MyLib\MathUtils\MathUtils.h"

#include "Game/Common/Collision/CollisionManager/CollisionManager.h"
#include "Game/Common/Collision/Collision.h"

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

	// ロープ同士の管理配列の領域を確保する
	m_ropeSegments.resize(particleNum - 1);

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

	for (size_t i = 0; i < particleNum; i++)
	{
		if (i == particleNum - 1) continue;

		// 処理用変数
		SimParticle* pSimParticle = &m_particles[i].simP;

		// 初期化処理
		m_ropeSegments[i] = std::make_unique<RopeSegment>(pSimParticle, &m_particles[i + 1].simP);
	}


	
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void XPBDSimulator::Update(float elapsedTime)
{

	// **** 固定点以外の位置をデータに反映 ****
	for (auto& particle : m_particles)
	{
		if (!particle.simP.IsFixed())  continue; 
		// 反映
		particle.simP.SetX(particle.pP->GetPosition());
	}

	Simulate(elapsedTime);

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
 * @param[in] elapsedTime　経過時間
 */
void XPBDSimulator::Simulate(float elapsedTime)
{
	// 予測位置の算出
	PredictNextPositions(elapsedTime);

	// 制約の生成
	GenerateConstraints();

	// 制約の初期化
	ResetConstraintParameters();

	// 各制約に対してXPBDの反復計算を行い、パーティクルの予測位置（xi）を調整する
	IterateConstraints(elapsedTime);

	// 予測位置と現在位置から速度を更新
	FinalizeVelocitiesAndPositions(elapsedTime);
}

/**
 * @brief リセットする
 * 
 */
void XPBDSimulator::Reset()
{
	m_particles.clear();
	m_ropeSegments.clear();
}

/**
 * @brief 各パーティクルの予測位置（xi）を慣性に基づいて計算する
 *
 * 現在位置と速度を使って、次のフレームでの予測位置を求めます。
 * 外力（重力など）はこの時点では加味しません。
 *
 * @param[in] elapsedTime 経過時間（Δt）
 */
void XPBDSimulator::PredictNextPositions(float elapsedTime)
{

	// x~ = x + Δt v + Δt^2 M^-1 f_ext(x)
	// **** 次の地点の予測 ****
	for (auto& particle : m_particles)
	{
		// 処理用変数
		auto* simP = &particle.simP;

		// 次の地点
		auto xTilda = simP->GetX() + elapsedTime * simP->GetV();
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
	for (auto& segment : m_ropeSegments)
	{
		segment->ResetConstraintParam(m_parameter.flexibility);
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
	m_constraints.clear();
	int i = 0;
	// 各パーティクルと地面の衝突判定を行う（単純な平面衝突の例）
	for (auto& particle : m_particles)
	{
		i++;

		// 処理用変数
		auto* simP = &particle.simP;

		// 固定されているパーティクルは衝突処理しない
		if (simP->IsFixed()) continue;


		
		Sphere sphere(simP->GetXi(), 0.3f);
		std::vector<const ICollider*> hitColliders;

		if (m_pCollisionManager->RetrieveCollisionData(&sphere, nullptr, &hitColliders) == false) continue ;

		if (hitColliders.size() <= 0) continue;

		// **** 合計押し出しの計算 ****
		SimpleMath::Vector3 overlap		= SimpleMath::Vector3::Zero;
		SimpleMath::Vector4 planeNormal = SimpleMath::Vector4::Zero;

		// 衝突したコライダーごとに処理
		for (auto& collider : hitColliders)
		{

			// 矩形コライダーと衝突
			if (collider->GetColliderType() == ColliderType::BOX2D)
			{
				if (const Box2D* box = dynamic_cast<const Box2D*>(collider))
				{
					overlap = CalcOverlap(*box, sphere);
					auto planeNormalTemp = box->GetPlane().GetVector4();

					auto moveDir = simP->GetV();
					moveDir.Normalize();

					auto planeNormal3 = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
					auto planeNormalTemp3 = SimpleMath::Vector3(planeNormalTemp.x, planeNormalTemp.y, planeNormalTemp.z);

					if (moveDir.Dot(planeNormal3) > moveDir.Dot(planeNormalTemp3))
						planeNormal = planeNormalTemp;
				}
			}
			// 平面コライダーと衝突
			else if (collider->GetColliderType() == ColliderType::Plane)
			{
				if (const Plane* plane = dynamic_cast<const Plane*>(collider))
				{
					overlap = CalcOverlap(*plane, sphere);
				}
			}
			// AABBコライダーと衝突
			else if (collider->GetColliderType() == ColliderType::AABB)
			{
				if (const AABB* box = dynamic_cast<const AABB*>(collider))
				{
					overlap = CalcOverlapNormal(*box, sphere);
					SimpleMath::Vector3 overlapNormal = overlap;
					overlapNormal.Normalize();

					//SimpleMath::Vector3 hitPos = box->GetCollisionNormal(*box, sphere);

					//Plane d = Plane(overlapNormal, hitPos);
					auto planeNormalTemp = box->GetCollisionNormal(*box, sphere);

					auto moveDir = simP->GetV();
					moveDir.Normalize();

					auto planeNormal3 = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
					auto planeNormalTemp3 = SimpleMath::Vector3(planeNormalTemp.x, planeNormalTemp.y, planeNormalTemp.z);

					if (moveDir.Dot(planeNormal3) > moveDir.Dot(planeNormalTemp3))
						planeNormal = planeNormalTemp;

				}
				
			}
		
				
		}
		if (MyLib::ApproxEqual(planeNormal.LengthSquared(), 0.0f)) continue;
		//planeNormal = -planeNormal;
		// パーティクルの予測位置が地面より下にあるかチェック
		// 地面は Y=0 の平面で、法線は (0, 1, 0) と仮定
		// C(x) = x・n - D
		//float penetrationDepth = simP->GetXi().Dot(m_groundPlaneNormal) - m_groundPlaneDistance;
		float penetrationDepth = overlap.Length();



		penetrationDepth = simP->GetXi().Dot(SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z) ) +  planeNormal.w;


		// 貫通している場合（penetrationDepth が負の値の場合）
		// 実際には、貫通深度が小さい負の値（例えば -epsilon）でも制約を適用して押し戻す
		if (penetrationDepth < 0.0f) // あるいは penetrationDepth < -0.001f など閾値を設ける
		{

			// 衝突制約を生成し、m_constraintsに追加
			// penetrationDepthが負の値の場合、制約はまだ違反していないとみなされるため
			// ComputeLambdaCorrection 内で C を正の値にする必要があるか確認が必要
			// 多くのPBD/XPBDの実装では C > 0 (拘束違反) の場合に制約を適用する
			// ここでは、パーティクルが平面より"下"にいることを違反とするため、EvaluateConstraint() が正になることを期待
			auto collisionConstraint= std::make_unique<CollisionConstraint>(simP);
			collisionConstraint->ResetConstraintParam(m_parameter.flexibility);
			collisionConstraint->m_planeDistance = planeNormal.w;
			collisionConstraint->m_collisionNormal = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
			
			m_constraints.push_back(std::move(collisionConstraint));
			
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
 * @param[in] elapsedTime 経過時間（Δt）
 */
void XPBDSimulator::IterateConstraints(float elapsedTime)
{
	using namespace SimpleMath;

	std::vector<IConstraint*> constraints;
	//constraints.resize(m_constraints.size() + m_ropeSegments.size());
	// コピーを取って使う
	//constraints.reserve(m_ropeSegments.size() + m_constraints.size());

	for (auto& seg : m_ropeSegments) 
	{
		constraints.push_back(seg.get());
	}
	for (auto& con : m_constraints) 
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
			float Δλ = constraint->ComputeLambdaCorrection(elapsedTime, C);

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
 * @param[in] elapsedTime 経過時間（Δt）
 */
void XPBDSimulator::FinalizeVelocitiesAndPositions(float elapsedTime)
{
	for (auto& particle : m_particles)
	{
		// 処理用変数
		auto* simP = &particle.simP;

		if (simP->IsFixed())  continue;

		// **** 考え中 : 速度の更新の仕方候補 *****

		// ① シンプルに重力も全部処理しちゃう
		// 速度の更新
		SimpleMath::Vector3 v_potential = ((simP->GetXi() - simP->GetX()) / elapsedTime + m_parameter.gravity * elapsedTime);
		simP->SetV(v_potential + m_parameter.gravity * elapsedTime);

		// ② 押し出しがあった場合押し出し方向の速度をなくす
		//SimpleMath::Vector3 v_potential = ((simP->GetXi() - simP->GetX()) / elapsedTime );
		//if (MyLib::ApproxEqual(particle.simP.m_planeNormal.LengthSquared(), 0.0f) == false)
		//{
		//	float dot_product = v_potential.Dot(particle.simP.m_planeNormal);
		//	// 法線方向の速度成分を削除
		//	v_potential -= dot_product * particle.simP.m_planeNormal;
		//	simP->SetV(v_potential);
		//}
		//else
			//simP->SetV(v_potential + m_parameter.gravity * elapsedTime);

		particle.simP.m_planeNormal = SimpleMath::Vector3::Zero;
		// 座標の更新
		// 予測点を現在の座標とする
		simP->SetX(simP->GetXi());
	}
}
