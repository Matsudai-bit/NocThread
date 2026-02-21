/*****************************************************************//**
 * @file    CollisionConstraintFactory.h
 * @brief   衝突制約の生成器に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionConstraintFactory.h"

// ライブラリ関連
#include "Library/MyLib/MathUtils/MathUtils.h"

// フレームワーク関連
#include "Game/Common/Framework/Collision/CollisionManager/CollisionManager.h"


// ユーティリティ関連
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/CollisionConstraint/CollisionConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
CollisionConstraintFactory::CollisionConstraintFactory(CollisionManager* pCollisionManager)
	: ConstraintFactoryBase		{ true }
	,m_pCollisionManager	{ pCollisionManager }
{
}



/**
 * @brief デストラクタ
 */
CollisionConstraintFactory::~CollisionConstraintFactory()
{

}

/**
 * @brief 衝突制約の作成
 * 
 * @param[in] pParticle->パーティクル
 * 
 * @return 作成した衝突制約
 */
std::vector<std::unique_ptr<IConstraint>> CollisionConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater)
{
	std::vector<std::unique_ptr<IConstraint>> creationConstraints;

	//for (auto& particle : *pParticles)
	//{
		SimParticle* simParticle = &(*pParticles).back().simP;

		// 固定されているパーティクルは衝突処理しない
		if (simParticle->IsFixed()) return creationConstraints;



		Sphere sphere(simParticle->GetXi(), 0.3f);
		std::vector<const ICollider*> hitColliders;

		if (m_pCollisionManager->RetrieveCollisionData(&sphere, nullptr, &hitColliders) == false) return creationConstraints;

		if (hitColliders.size() <= 0) return creationConstraints;

		// **** 合計押し出しの計算 ****
		SimpleMath::Vector3 overlap = SimpleMath::Vector3::Zero;
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

					auto moveDir = simParticle->GetV();
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

					auto moveDir = simParticle->GetV();
					moveDir.Normalize();

					auto planeNormal3 = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
					auto planeNormalTemp3 = SimpleMath::Vector3(planeNormalTemp.x, planeNormalTemp.y, planeNormalTemp.z);

					if (moveDir.Dot(planeNormal3) > moveDir.Dot(planeNormalTemp3))
						planeNormal = planeNormalTemp;

				}

			}


		}
		if (MyLib::ApproxEqual(planeNormal.LengthSquared(), 0.0f)) return creationConstraints;
		//planeNormal = -planeNormal;
		// パーティクルの予測位置が地面より下にあるかチェック
		// 地面は Y=0 の平面で、法線は (0, 1, 0) と仮定
		// C(x) = x・n - D
		//float penetrationDepth = simParticle->GetXi().Dot(m_groundPlaneNormal) - m_groundPlaneDistance;
		float penetrationDepth = overlap.Length();



		penetrationDepth = simParticle->GetXi().Dot(SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z)) + planeNormal.w;


		// 貫通している場合（penetrationDepth が負の値の場合）
		// 実際には、貫通深度が小さい負の値（例えば -epsilon）でも制約を適用して押し戻す
		if (penetrationDepth < 0.0f) // あるいは penetrationDepth < -0.001f など閾値を設ける
		{

			// 衝突制約を生成し、m_constraintsに追加
			// penetrationDepthが負の値の場合、制約はまだ違反していないとみなされるため
			// ComputeLambdaCorrection 内で C を正の値にする必要があるか確認が必要
			// 多くのPBD/XPBDの実装では C > 0 (拘束違反) の場合に制約を適用する
			// ここでは、パーティクルが平面より"下"にいることを違反とするため、EvaluateConstraint() が正になることを期待
			auto collisionConstraint = std::make_unique<CollisionConstraint>(simParticle);
			collisionConstraint->ResetConstraintParam(paramater.flexibility);
			collisionConstraint->m_planeDistance = planeNormal.w;
			collisionConstraint->m_collisionNormal = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);

			creationConstraints.emplace_back(std::move(collisionConstraint));
			

		}
	
	return creationConstraints;
	
}

//
///**
// * @brief 衝突制約の作成
// *
// * @param[in] pParticle->パーティクル
// *
// * @return 作成した衝突制約
// */
//std::vector<std::unique_ptr<IConstraint>> CollisionConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater)
//{
//	std::vector<std::unique_ptr<IConstraint>> creationConstraints;
//
//	for (auto& particle : *pParticles)
//	{
//		SimParticle* simParticle = &particle.simP;
//
//		// 固定されているパーティクルは衝突処理しない
//		if (simParticle->IsFixed()) continue;
//
//
//
//		Sphere sphere(simParticle->GetXi(), 0.3f);
//		std::vector<const ICollider*> hitColliders;
//
//		if (m_pCollisionManager->RetrieveCollisionData(&sphere, nullptr, &hitColliders) == false) continue;
//
//		if (hitColliders.size() <= 0) continue;
//
//		// **** 合計押し出しの計算 ****
//		SimpleMath::Vector3 overlap = SimpleMath::Vector3::Zero;
//		SimpleMath::Vector4 planeNormal = SimpleMath::Vector4::Zero;
//
//		// 衝突したコライダーごとに処理
//		for (auto& collider : hitColliders)
//		{
//
//			// 矩形コライダーと衝突
//			if (collider->GetColliderType() == ColliderType::BOX2D)
//			{
//				if (const Box2D* box = dynamic_cast<const Box2D*>(collider))
//				{
//					overlap = CalcOverlap(*box, sphere);
//					auto planeNormalTemp = box->GetPlane().GetVector4();
//
//					auto moveDir = simParticle->GetV();
//					moveDir.Normalize();
//
//					auto planeNormal3 = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
//					auto planeNormalTemp3 = SimpleMath::Vector3(planeNormalTemp.x, planeNormalTemp.y, planeNormalTemp.z);
//
//					if (moveDir.Dot(planeNormal3) > moveDir.Dot(planeNormalTemp3))
//						planeNormal = planeNormalTemp;
//				}
//			}
//			// 平面コライダーと衝突
//			else if (collider->GetColliderType() == ColliderType::Plane)
//			{
//				if (const Plane* plane = dynamic_cast<const Plane*>(collider))
//				{
//					overlap = CalcOverlap(*plane, sphere);
//				}
//			}
//			// AABBコライダーと衝突
//			else if (collider->GetColliderType() == ColliderType::AABB)
//			{
//				if (const AABB* box = dynamic_cast<const AABB*>(collider))
//				{
//					overlap = CalcOverlapNormal(*box, sphere);
//					SimpleMath::Vector3 overlapNormal = overlap;
//					overlapNormal.Normalize();
//
//					//SimpleMath::Vector3 hitPos = box->GetCollisionNormal(*box, sphere);
//
//					//Plane d = Plane(overlapNormal, hitPos);
//					auto planeNormalTemp = box->GetCollisionNormal(*box, sphere);
//
//					auto moveDir = simParticle->GetV();
//					moveDir.Normalize();
//
//					auto planeNormal3 = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
//					auto planeNormalTemp3 = SimpleMath::Vector3(planeNormalTemp.x, planeNormalTemp.y, planeNormalTemp.z);
//
//					if (moveDir.Dot(planeNormal3) > moveDir.Dot(planeNormalTemp3))
//						planeNormal = planeNormalTemp;
//
//				}
//
//			}
//
//
//		}
//		if (MyLib::ApproxEqual(planeNormal.LengthSquared(), 0.0f)) continue;
//		//planeNormal = -planeNormal;
//		// パーティクルの予測位置が地面より下にあるかチェック
//		// 地面は Y=0 の平面で、法線は (0, 1, 0) と仮定
//		// C(x) = x・n - D
//		//float penetrationDepth = simParticle->GetXi().Dot(m_groundPlaneNormal) - m_groundPlaneDistance;
//		float penetrationDepth = overlap.Length();
//
//
//
//		penetrationDepth = simParticle->GetXi().Dot(SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z)) + planeNormal.w;
//
//
//		// 貫通している場合（penetrationDepth が負の値の場合）
//		// 実際には、貫通深度が小さい負の値（例えば -epsilon）でも制約を適用して押し戻す
//		if (penetrationDepth < 0.0f) // あるいは penetrationDepth < -0.001f など閾値を設ける
//		{
//
//			// 衝突制約を生成し、m_constraintsに追加
//			// penetrationDepthが負の値の場合、制約はまだ違反していないとみなされるため
//			// ComputeLambdaCorrection 内で C を正の値にする必要があるか確認が必要
//			// 多くのPBD/XPBDの実装では C > 0 (拘束違反) の場合に制約を適用する
//			// ここでは、パーティクルが平面より"下"にいることを違反とするため、EvaluateConstraint() が正になることを期待
//			auto collisionConstraint = std::make_unique<CollisionConstraint>(simParticle);
//			collisionConstraint->ResetConstraintParam(paramater.flexibility);
//			collisionConstraint->m_planeDistance = planeNormal.w;
//			collisionConstraint->m_collisionNormal = SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z);
//
//			creationConstraints.emplace_back(std::move(collisionConstraint));
//
//
//		}
//	}
//	return creationConstraints;
//
//}