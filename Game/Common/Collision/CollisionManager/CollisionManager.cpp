/**
 * @file   CollisionManager.cpp
 *
 * @brief  当たり判定管理に関するソースファイル
 *
 * @author 松下大暉
 *
 * @date   2025/03/22
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "CollisionManager.h"
#include <set>



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CollisionManager::CollisionManager()
	:m_collisionData{}
{
	
}



/**
 * @brief デストラクタ
 */
CollisionManager::~CollisionManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CollisionManager::Initialize()
{

}




/**
 * @brief タスクの更新処理
 * 
 * @param[in] deltaTime　前フレームとの時間差
 * 
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool CollisionManager::UpdateTask(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// 衝突判定の流れ
	// 衝突判定->衝突処理->押し出し
	if (m_collisionData.empty() || m_collisionData.size() == 0) return true;

	// 事前処理
	PreCollision();

	// 衝突したオブジェクト情報群
	std::vector<DetectedCollisonData> detectedCollisions{};

	// 衝突を検知する
	UpdateDetection(&detectedCollisions);

	// 衝突を通知する
	NotifyCollisionEvents(&detectedCollisions);

	// 事後処理
	FinalizeCollision();

	// タスクを継続する
	return true;
}




/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */	
void CollisionManager::Draw()
{

}




/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CollisionManager::Finalize()
{
	for (auto& data : m_collisionData)
	{
		data.pGameObject = nullptr;
		data.pCollider = nullptr;
	}

	m_collisionData.clear();
}


void CollisionManager::AddCollisionData(const CollisionData& collisionData)
{
	// 衝突判定データの追加
	m_collisionData.push_back(collisionData);
}

/**
 * @brief 削除
 * 
 * @param[in] pAddGameObject
 * @param[in] pCollider
 */
void CollisionManager::RemoveCollisionObjectData(GameObject* pAddGameObject, ICollider* pCollider)
{
	if (pAddGameObject == nullptr || pCollider == nullptr) return;

	// 探す
	auto findIt = std::find_if(m_collisionData.begin(), m_collisionData.end(),
		[&](const CollisionData& data)
		{
			return (pAddGameObject == data.pGameObject && pCollider == data.pCollider);
		});

	if (findIt == m_collisionData.end()) return;

	// 削除
	m_collisionData.erase(findIt);
}


/**
 * @brief 何かのオブジェクトと当たったかどうか
 *
 * @param[in] checkCollider 衝突判定を検証するコライダー
 *
 * @return true 当たっている
 */
bool CollisionManager::IsCollisionWithAnyObject(const ICollider& checkCollider)
{
	for (size_t i = 0; i < m_collisionData.size(); i++)
	{
		// 処理用オブジェクト
		GameObject* gameObject = m_collisionData[i].pGameObject;

		//活動していなければ飛ばす
		if (gameObject->IsActive() == false) continue;


		// 衝突しているかどうか
		if (DetectCollision(&checkCollider, m_collisionData[i].pCollider))
		{
			// 衝突していればtrueを返す
			return true;
		}
	}

	return false;
}

/**
 * @brief 衝突判定
 *
 * @param[in] pColliderA コライダーA
 * @param[in] pColliderB コライダーB
 *
 * @return true 衝突している
 */
bool CollisionManager::DetectCollision(const ICollider* pColliderA, const ICollider* pColliderB)
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



/**
 * @brief 指定したオブジェクトの種類と衝突しているかどうか
 *
 * @param[in] pCheckCollider
 * @param[in] checkTag
 *
 * @return true 衝突している
 */
bool CollisionManager::IsCollidingWithObjectType(const ICollider* pCheckCollider, const GameObjectTag& checkTag)
{

	for (auto& collInfo : m_collisionData)
	{
		// 衝突判定
		if (DetectCollision(collInfo.pCollider, pCheckCollider))
		{
			// タグと比べる
			if (collInfo.pGameObject->GetTag() == checkTag)
				return true;
		}
	}

	return false;
}

/**
 * @brief 指定したコライダーの衝突情報を取得
 * 
 * @param[in] pCheckCollider	調べるコライダー
 * @param[in] pHitGameObjects	衝突データ
 * 
 * @return true 衝突しているかどうか
 */
bool CollisionManager::RetrieveCollisionData(const ICollider* pCheckCollider, std::vector<const GameObject*>* pHitGameObjects, std::vector<const ICollider*>* pHitColliders)
{
	bool isHit = false;

	for (auto& collInfo : m_collisionData)
	{
		// 衝突判定
		if (DetectCollision(collInfo.pCollider, pCheckCollider))
		{
			// 登録
			if (pHitGameObjects)
				pHitGameObjects->push_back(collInfo.pGameObject);
			if (pHitColliders)
				pHitColliders->push_back(collInfo.pCollider);

			isHit = true;
		}
	}

	return isHit;
}

/**
 * @brief 衝突判定直前処理
 */
void CollisionManager::PreCollision()
{
	// 衝突判定の前の処理をする
	for (auto& data : m_collisionData)
	{
		if (data.pGameObject)
		{
			data.pGameObject->PreCollision();
		}
	}
}

/**
 * @brief 衝突の検知
 * 
 * @param[out] pOutResults 検知された衝突
 */
void CollisionManager::UpdateDetection(std::vector<DetectedCollisonData>* pOutResults)
{

	for (size_t i = 0; i < m_collisionData.size() - 1; i++)
	{

		for (size_t j = i + 1; j < m_collisionData.size(); j++)
		{
			CheckCollisionPair(m_collisionData[i], m_collisionData[j], pOutResults);
		}
	}
}

/**
 * @brief 衝突の通知
 *
 * @param[in] pDetectedCollisions 検知された衝突
 */
void CollisionManager::NotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions)
{
	// **** 衝突したオブジェクトの衝突処理 ****
	for (int i = 0; i < pDetectedCollisions->size(); i++)
	{
		// ゲームオブジェクトの取得
		GameObject* pGameObjectA = (*pDetectedCollisions)[i].pCollisionDataA->pGameObject;
		GameObject* pGameObjectB = (*pDetectedCollisions)[i].pCollisionDataB->pGameObject;

		if (!pGameObjectA || !pGameObjectB) { continue; }

		// コライダーの取得
		ICollider* pColliderA = (*pDetectedCollisions)[i].pCollisionDataA->pCollider;
		ICollider* pColliderB = (*pDetectedCollisions)[i].pCollisionDataB->pCollider;

		// 衝突処理
		pGameObjectA->OnCollision(CollisionInfo(pColliderB, pGameObjectB, pColliderA));
		pGameObjectB->OnCollision(CollisionInfo(pColliderA, pGameObjectA, pColliderB));
	}
}

/**
 * @brief 事後処理
 */
void CollisionManager::FinalizeCollision()
{
	// 衝突判定の直後の処理をする
	for (auto& data : m_collisionData)
	{
		if (data.pGameObject)
		{
			data.pGameObject->PostCollision();

		}

	}

}

/**
 * @brief ペアの衝突チェック
 * 
 * @param[in] collisionDataA	衝突データA
 * @param[in] collisionDataB	衝突データB
 * 
 * @param[out] pOutResults 検知された衝突を格納する
 */
void CollisionManager::CheckCollisionPair(const CollisionData& collisionDataA, const CollisionData& collisionDataB, std::vector<DetectedCollisonData>* pOutResults)
{
	// 処理用オブジェクト
	GameObject* gameObjectA = collisionDataA.pGameObject;

	// 活動していなければ飛ばす
	if (gameObjectA && gameObjectA->IsActive() == false) return;


	// 処理用オブジェクト
	GameObject* gameObjectB = collisionDataB.pGameObject;


	// ゲームオブジェクトタグのビットインデックス
	uint32_t gameObjectTagIndexA = collisionDataA.tagBitIndex;
	// 衝突検知をするかどうか
	if (gameObjectA && gameObjectB && m_pCollisionMatrix &&
		!m_pCollisionMatrix->ShouldCollide(gameObjectTagIndexA, gameObjectB->GetTag())) {
		return;
	}

	//活動していなければ飛ばす
	if (gameObjectB && gameObjectB->IsActive() == false) return;

	if (gameObjectA == gameObjectB) return;


	// 衝突しているかどうか
	if (DetectCollision(collisionDataA.pCollider, collisionDataB.pCollider))
	{
		// 衝突検知された情報を登録
		pOutResults->push_back({ &collisionDataA, &collisionDataB });

		// 子を持っている場合その子も検知する
		for (auto& child : collisionDataA.children)
		{
			CheckCollisionPair(child, collisionDataB, pOutResults);
		}
		for (auto& child : collisionDataB.children)
		{
			CheckCollisionPair(child, collisionDataA, pOutResults);
		}
	}

}





