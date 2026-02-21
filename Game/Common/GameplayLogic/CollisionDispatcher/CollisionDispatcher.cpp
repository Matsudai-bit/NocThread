/*****************************************************************//**
 * @file    CollisionDispatcher.h
 * @brief   コライダーごとの衝突判定をするクラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/02/21
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionDispatcher.h"
#include "Game/Common/Utillities/Collision/ICollision.h"
#include "Game/Common/Utillities/Collision/Collision.h"



// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連

// ゲームプレイロジック関連

// グラフィック関連


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CollisionDispatcher::CollisionDispatcher()
{

}



/**
 * @brief デストラクタ
 */
CollisionDispatcher::~CollisionDispatcher()
{

}


/**
 * @brief 衝突判定
 *
 * @param[in] pColliderA コライダーA
 * @param[in] pColliderB コライダーB
 *
 * @return true 衝突している
 */
bool CollisionDispatcher::DetectCollision(const ICollider* pColliderA, const ICollider* pColliderB)
{
	// コライダーの種類が平面なら
	if (pColliderA->GetColliderType() == ColliderType::Plane)
	{
		// 平面にキャスト
		const Plane* pPlane = dynamic_cast<const Plane*>(pColliderA);
		if (pPlane == nullptr) return false;

		// 片方のコライダーの種類が球なら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// 球にキャスト
			const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphere == nullptr) return false;

			return (pPlane->CheckHit(*pSphere));

		}

		// 片方のコライダーの種類が線分なら
		if (pColliderB->GetColliderType() == ColliderType::Segment)
		{
			// 線分にキャスト
			const Segment* pSegment = dynamic_cast<const Segment*>(pColliderB);
			if (pSegment == nullptr) return false;

			return (pPlane->CheckHit(*pSegment));

		}
	}

	// コライダーの種類が球なら
	if (pColliderA->GetColliderType() == ColliderType::Sphere)
	{
		// 平面にキャスト
		const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderA);
		if (pSphere == nullptr) return false;

		// 片方のコライダーの種類が平面なら
		if (pColliderB->GetColliderType() == ColliderType::Plane)
		{
			// 球にキャスト
			const Plane* pPlane = dynamic_cast<const Plane*>(pColliderB);
			if (pPlane == nullptr) return false;

			return (pSphere->CheckHit(*pPlane));

		}

		// 片方のコライダーの種類が線分なら
		if (pColliderB->GetColliderType() == ColliderType::Segment)
		{
			// 線分にキャスト
			const Segment* pSegment = dynamic_cast<const Segment*>(pColliderB);
			if (pSegment == nullptr) return false;

			return (pSphere->CheckHit(*pSegment));
		}
		// 片方のコライダーの種類が三角形なら
		if (pColliderB->GetColliderType() == ColliderType::Triangle)
		{
			// 線分にキャスト
			const Triangle* pTriangle = dynamic_cast<const Triangle*>(pColliderB);
			if (pTriangle == nullptr) return false;

			return (pSphere->CheckHit(*pTriangle));
		}

		// 片方のコライダーの種類が三角形なら
		if (pColliderB->GetColliderType() == ColliderType::BOX2D)
		{
			// 線分にキャスト
			const Box2D* pBox = dynamic_cast<const Box2D*>(pColliderB);
			if (pBox == nullptr) return false;

			return (pSphere->CheckHit(*pBox));
		}

		// 片方のコライダーの種類が球なら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// 線分にキャスト
			const Sphere* pSphereB = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphereB == nullptr) return false;

			return (pSphere->CheckHit(*pSphereB));
		}

		// 片方のコライダーのAABBなら
		if (pColliderB->GetColliderType() == ColliderType::AABB)
		{
			// AABBにキャスト
			const AABB* pAABB = dynamic_cast<const AABB*>(pColliderB);
			if (pAABB == nullptr) return false;

			return (pSphere->CheckHit(*pAABB));
		}
		// 片方のコライダーのカプセルなら
		if (pColliderB->GetColliderType() == ColliderType::Capsule)
		{
			// Capsuleにキャスト
			const Capsule* pCapsule = dynamic_cast<const Capsule*>(pColliderB);
			if (pCapsule == nullptr) return false;

			return (pSphere->CheckHit(*pCapsule));
		}
	}

	// コライダーの種類が三角形なら
	if (pColliderA->GetColliderType() == ColliderType::Triangle)
	{
		// 三角形にキャスト
		const Triangle* pTriangle = dynamic_cast<const Triangle*>(pColliderA);
		if (pTriangle == nullptr) return false;


		// 片方のコライダーの種類が球なら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// 球にキャスト
			const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphere == nullptr) return false;

			return (pTriangle->CheckHit(*pSphere));

		}

		// 片方のコライダーの種類が線分なら
		if (pColliderB->GetColliderType() == ColliderType::Segment)
		{
			// 線分にキャスト
			const Segment* pSegment = dynamic_cast<const Segment*>(pColliderB);
			if (pSegment == nullptr) return false;

			return (pTriangle->CheckHit(*pSegment));

		}
	}

	// コライダーの種類が四角形なら
	if (pColliderA->GetColliderType() == ColliderType::BOX2D)
	{
		// 三角形にキャスト
		const Box2D* pBox = dynamic_cast<const Box2D*>(pColliderA);
		if (pBox == nullptr) return false;


		// 片方のコライダーの種類が球なら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// 球にキャスト
			const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphere == nullptr) return false;

			return (pBox->CheckHit(*pSphere));

		}

		// 片方のコライダーの種類が線分なら
		if (pColliderB->GetColliderType() == ColliderType::Segment)
		{
			// 線分にキャスト
			const Segment* pSegment = dynamic_cast<const Segment*>(pColliderB);
			if (pSegment == nullptr) return false;

			return (pBox->CheckHit(*pSegment));

		}
	}

	// コライダーの種類が線分なら
	if (pColliderA->GetColliderType() == ColliderType::Segment)
	{
		// 三角形にキャスト
		const Segment* pSegment = dynamic_cast<const Segment*>(pColliderA);
		if (pSegment == nullptr) return false;


		// 片方のコライダーの種類が平面なら
		if (pColliderB->GetColliderType() == ColliderType::Plane)
		{
			// 球にキャスト
			const Plane* pPlane = dynamic_cast<const Plane*>(pColliderB);
			if (pPlane == nullptr) return false;

			return (pSegment->CheckHit(*pPlane));

		}


		// 片方のコライダーの種類が三角形なら
		if (pColliderB->GetColliderType() == ColliderType::Triangle)
		{
			// 線分にキャスト
			const Triangle* pTriangle = dynamic_cast<const Triangle*>(pColliderB);
			if (pTriangle == nullptr) return false;

			return (pSegment->CheckHit(*pTriangle));
		}

		// 片方のコライダーの種類が四角形なら
		if (pColliderB->GetColliderType() == ColliderType::BOX2D)
		{
			// 線分にキャスト
			const Box2D* pBox = dynamic_cast<const Box2D*>(pColliderB);
			if (pBox == nullptr) return false;

			return (pSegment->CheckHit(*pBox));
		}

		// 片方のコライダーの種類が球なら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// 線分にキャスト
			const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphere == nullptr) return false;

			return (pSegment->CheckHit(*pSphere));
		}


		// 片方のコライダーのAABBなら
		if (pColliderB->GetColliderType() == ColliderType::AABB)
		{
			// AABBにキャスト
			const AABB* pAABB = dynamic_cast<const AABB*>(pColliderB);
			if (pAABB == nullptr) return false;

			return (pSegment->CheckHit(*pAABB));
		}

	}

	// コライダーの種類がAABBなら
	if (pColliderA->GetColliderType() == ColliderType::AABB)
	{	// AABBにキャスト
		const AABB* pAABB = dynamic_cast<const AABB*>(pColliderA);

		// 片方のコライダーのAABBなら
		if (pColliderB->GetColliderType() == ColliderType::AABB)
		{
			// AABBにキャスト
			const AABB* pAABB_B = dynamic_cast<const AABB*>(pColliderB);
			if (pAABB_B == nullptr) return false;

			return (pAABB->CheckHit(*pAABB_B));
		}
		// 片方のコライダーの種類が線分なら
		if (pColliderB->GetColliderType() == ColliderType::Segment)
		{
			// 線分にキャスト
			const Segment* pSegment = dynamic_cast<const Segment*>(pColliderB);
			if (pSegment == nullptr) return false;

			return (pAABB->CheckHit(*pSegment));

		}
		// 片方のコライダーの種類が球なら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// 球にキャスト
			const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphere == nullptr) return false;

			return (pAABB->CheckHit(*pSphere));

		}

		// 片方のコライダーの種類が筒なら
		if (pColliderB->GetColliderType() == ColliderType::Cylinder)
		{
			// 筒にキャスト
			const Cylinder* pCylinder = dynamic_cast<const Cylinder*>(pColliderB);
			if (pCylinder == nullptr) return false;

			return (pAABB->CheckHit(*pCylinder));

		}

		// 片方のコライダーの種類がカプセルなら
		if (pColliderB->GetColliderType() == ColliderType::Capsule)
		{
			// カプセルにキャスト
			const Capsule* pCapsule = dynamic_cast<const Capsule*>(pColliderB);
			if (pCapsule == nullptr) return false;

			return (pAABB->CheckHit(*pCapsule));

		}
	}

	// コライダーの種類が筒なら
	if (pColliderA->GetColliderType() == ColliderType::Cylinder)
	{	// AABBにキャスト
		const Cylinder* pCylinder = dynamic_cast<const Cylinder*>(pColliderA);

		// 片方のコライダーのAABBなら
		if (pColliderB->GetColliderType() == ColliderType::AABB)
		{
			// AABBにキャスト
			const AABB* pAABB = dynamic_cast<const AABB*>(pColliderB);
			if (pAABB == nullptr) return false;

			return (pAABB->CheckHit(*pCylinder));
		}
	}

	// コライダーの種類がカプセルなら
	if (pColliderA->GetColliderType() == ColliderType::Capsule)
	{	// AABBにキャスト
		const Capsule* pCapsule = dynamic_cast<const Capsule*>(pColliderA);

		// 片方のコライダーのSphereなら
		if (pColliderB->GetColliderType() == ColliderType::Sphere)
		{
			// Sphereにキャスト
			const Sphere* pSphere = dynamic_cast<const Sphere*>(pColliderB);
			if (pSphere == nullptr) return false;

			return (pSphere->CheckHit(*pCapsule));
		}

		// 片方のコライダーのAABBなら
		if (pColliderB->GetColliderType() == ColliderType::AABB)
		{
			// AABBにキャスト
			const AABB* pAABB = dynamic_cast<const AABB*>(pColliderB);
			if (pAABB == nullptr) return false;

			return (pAABB->CheckHit(*pCapsule));
		}
	}

	return false;
}
