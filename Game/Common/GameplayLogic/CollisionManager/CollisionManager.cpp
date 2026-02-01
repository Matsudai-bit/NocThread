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
	: m_rootCollisionDataId{}
	, m_pCollisionMatrix{ nullptr }
	, m_applyThread{ false }
	, m_isCalculating{ false }
	, m_stopThread{ false }
{
	 m_proxy.resize(1336);

	m_detectionThread = std::make_unique<std::thread>(&CollisionManager::UpdateDetection, this, &m_proxy, &m_detectionResults);

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
	// 1. 開始時刻の記録
	auto start = std::chrono::high_resolution_clock::now();

	
	// 衝突を通知する
	NotifyCollisionEvents(&m_detectionResults);


	// 事後処理
	FinalizeCollision();

	//// 3. 終了時刻の記録
auto end = std::chrono::high_resolution_clock::now();

	// 4. 処理時間の計算と表示
// duration_cast で希望の単位に変換（ここではマイクロ秒 ?s）
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

wchar_t buffer[64];
swprintf_s(buffer, L"duration : %lld us\n", duration.count());

// 3. Send to the Output window
OutputDebugString(buffer);
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
	m_rootCollisionDataId.clear();
	m_collisionIdLookupTable.clear();

	m_stopThread = true;
	m_cv.notify_one();
	m_detectionThread->join();

}


void CollisionManager::AddCollisionData(const CollisionData& collisionData)
{

	auto newId = RegisterIdLookUpTable(collisionData);

	std::lock_guard<std::mutex> lock{ m_mutex };
	m_rootCollisionDataId.push_back(newId);

}

void CollisionManager::AddCollisionData(const CollisionData& childCollisionData, ICollider* parent)
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	if (parent)
	{
		auto it = std::find_if(m_rootCollisionDataId.begin(), m_rootCollisionDataId.end(), [&](const unsigned int& id)
			{
				return  m_collisionIdLookupTable[id].pCollider == parent;
			});

		if (it != m_rootCollisionDataId.end())
		{
			auto& parentData = m_collisionIdLookupTable[*it];
			// 識別番号の設定
			auto newId = RegisterIdLookUpTable(childCollisionData);

			parentData.children.push_back(newId);

		}
	}
}

/**
 * @brief 削除
 *
 * @param[in] pAddGameObject
 * @param[in] pCollider
 */
void CollisionManager::RemoveCollisionObjectData(GameObject* pAddGameObject, ICollider* pCollider)
{
	RemoveAll();
	//if (!pAddGameObject || !pCollider) return;

	//// ここで一回だけロック！
	//std::lock_guard<std::mutex> lock{ m_mutex };

	//auto findIt = std::find_if(m_rootCollisionDataId.begin(), m_rootCollisionDataId.end(),
	//	[&](const UINT& id) {
	//		auto it = m_collisionIdLookupTable.find(id);
	//		if (it == m_collisionIdLookupTable.end()) return false;
	//		return (pAddGameObject == it->second.pGameObject && pCollider == it->second.pCollider);
	//	});

	//if (findIt != m_rootCollisionDataId.end()) {
	//	UINT idToRemove = *findIt;

	//	// 内部用関数を呼ぶ（再帰の中で二重ロックしないように）
	//	UnregisterIdLookUpTable(idToRemove);

	//	// IDのリサイクル（※本来は遅延返却が望ましい）
	//	m_idPool.RecycleID(idToRemove);
	//}
}

void CollisionManager::RemoveAll()
{
	std::lock_guard<std::mutex> lock{ m_mutex };

	m_collisionIdLookupTable.clear();
	m_rootCollisionDataId.clear();
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
	//for (size_t i = 0; i < m_rootCollisionDataId.size(); i++)
	//{
	//	// 処理用オブジェクト
	//	GameObject* gameObject = m_rootCollisionDataId[i].pGameObject;

	//	//活動していなければ飛ばす
	//	if (gameObject->IsActive() == false) continue;


	//	// 衝突しているかどうか
	//	if (DetectCollision(&checkCollider, m_rootCollisionDataId[i].pCollider))
	//	{
	//		// 衝突していればtrueを返す
	//		return true;
	//	}
	//}

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

	//for (auto& collInfo : m_rootCollisionDataId)
	//{
	//	// 衝突判定
	//	if (DetectCollision(collInfo.pCollider, pCheckCollider))
	//	{
	//		// タグと比べる
	//		if (collInfo.pGameObject->GetTag() == checkTag)
	//			return true;
	//	}
	//}

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

	//for (auto& collInfo : m_rootCollisionDataId)
	//{
	//	// 衝突判定
	//	if (DetectCollision(collInfo.pCollider, pCheckCollider))
	//	{
	//		// 登録
	//		if (pHitGameObjects)
	//			pHitGameObjects->push_back(collInfo.pGameObject);
	//		if (pHitColliders)
	//			pHitColliders->push_back(collInfo.pCollider);

	//		isHit = true;
	//	}
	//}

	return isHit;
}

void CollisionManager::StartThread()
{
	// 衝突判定の流れ
// 衝突判定->衝突処理->押し出し
	if (m_rootCollisionDataId.empty() || m_rootCollisionDataId.size() == 0) return ;


	// 事前処理
	PreCollision();

	// 判定用データの作成（ここはメインスレッドで安全に行う）
	std::vector<ThreadCollisionObjectProxy> nextProxy;
	CreateThreadCollisionObjectProxy(&nextProxy);

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_proxy = std::move(nextProxy); // 共有変数へムーブ
		m_detectionResults.clear();
		m_applyThread = true;
	}
	m_cv.notify_one(); // サブスレッドを起こす
}

/**
 * @brief 衝突判定直前処理
 */
void CollisionManager::PreCollision()
{
	// 衝突判定の前の処理をする
	for (auto& id : m_rootCollisionDataId)
	{
		auto pGameObject = m_collisionIdLookupTable.at(id).pGameObject;
		if (pGameObject)
		{
			pGameObject->PreCollision();
		}
	}
}

/**
 * @brief 衝突の検知
 *
 * @param[out] pOutResults 検知された衝突
 */
void CollisionManager::UpdateDetection(const std::vector<ThreadCollisionObjectProxy>* proxy, std::vector<DetectedCollisonData>* pOutResults)
{
	while (!m_stopThread)
	{
		std::vector<ThreadCollisionObjectProxy> localProxy;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            // 「仕事が来る(m_applyThread)」か「終了する(m_stopThread)」までスリープ
            m_cv.wait(lock, [this] { return m_applyThread || m_stopThread; });

            if (m_stopThread) break;

            // 判定に使うデータをローカルにコピー（メインスレッドとの干渉を防ぐ）
			localProxy = std::move(m_proxy);
			m_isCalculating = true;
            m_applyThread = false; // 受付完了
        }

        // --- ロックの外で重い判定処理を実行 ---
        if (localProxy.size() >= 2)
        {
            for (size_t i = 0; i < localProxy.size() - 1; i++)
            {
                for (size_t j = i + 1; j < localProxy.size(); j++)
                {
                    CheckCollisionPair(localProxy[i], localProxy[j], pOutResults);
                }
            }
        }
        
        m_isCalculating = false; // 計算終了
		
	
	}
}

/**
 * @brief 衝突の通知
 *
 * @param[in] pDetectedCollisions 検知された衝突
 */
void CollisionManager::NotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions)
{
	while (true)
	{
		DetectedCollisonData data;
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			// 1. データが空なら、計算中かどうかで続行か終了か決める
			if (pDetectedCollisions->empty()) {
				if (!m_isCalculating) break; // 計算も終わっていて空なら終了
				std::this_thread::yield();   // まだ計算中なら少し待って再トライ
				continue;
			}

			// 2. 安全にデータを取り出す
			data = pDetectedCollisions->back();
			pDetectedCollisions->pop_back();
		}

		// 3. IDがまだ存在するか「find」でチェック（[]は厳禁）
		auto itA = m_collisionIdLookupTable.find(data.collisionDataIdA);
		auto itB = m_collisionIdLookupTable.find(data.collisionDataIdB);

		if (itA != m_collisionIdLookupTable.end() && itB != m_collisionIdLookupTable.end())
		{
			CollisionData& collA = itA->second;
			CollisionData& collB = itB->second;

			if (collA.pGameObject && collB.pGameObject)
			{
				// ここでようやく発火
				collA.pGameObject->OnCollision(CollisionInfo(collB.pCollider, collB.pGameObject, collA.pCollider));
				collB.pGameObject->OnCollision(CollisionInfo(collA.pCollider, collA.pGameObject, collB.pCollider));
			}
		}
	}
}

/**
 * @brief 事後処理
 */
void CollisionManager::FinalizeCollision()
{
	// 衝突判定の直後の処理をする
	for (auto& data : m_collisionIdLookupTable)
	{
		if (data.second.pGameObject)
		{
			data.second.pGameObject->PostCollision();

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
void CollisionManager::CheckCollisionPair(const ThreadCollisionObjectProxy& collisionDataA, const ThreadCollisionObjectProxy& collisionDataB, std::vector<DetectedCollisonData>* pOutResults)
{
	if (collisionDataA.id == collisionDataB.id) return;
	
	// 活動していなければ飛ばす
	if (collisionDataA.isActive == false) return;
	//活動していなければ飛ばす
	if (collisionDataB.isActive == false) return;




	// ゲームオブジェクトタグのビットインデックス
	uint32_t gameObjectTagIndexA = collisionDataA.tagBitIndex;
	// 衝突検知をするかどうか
	if ( m_pCollisionMatrix &&
		!m_pCollisionMatrix->ShouldCollide(gameObjectTagIndexA, collisionDataB.tag)) {
		return;
	}

	// 衝突しているかどうか
	if (DetectCollision(collisionDataA.collider.get(), collisionDataB.collider.get()))
	{
		m_mutex.lock();
		// 衝突検知された情報を登録
		pOutResults->push_back({ collisionDataA.id, collisionDataB.id });
		m_mutex.unlock();

		// 子を持っている場合その子も検知する
		for (auto& child : collisionDataA.children)
		{
			CheckCollisionPair( child, collisionDataB, pOutResults);
		}
		for (auto& child : collisionDataB.children)
		{
			CheckCollisionPair(child, collisionDataA, pOutResults);
		}
	}

}

UINT CollisionManager::RegisterIdLookUpTable(CollisionData data)
{
	auto newId = m_idPool();
	data.id = newId;



	m_collisionIdLookupTable[data.id]  = data;

	return newId;

}

void CollisionManager::UnregisterIdLookUpTable(int dataID)
{
	auto it = m_collisionIdLookupTable.find(dataID);
	if (it == m_collisionIdLookupTable.end()) return;

	// 1. 先に子供のIDリストをコピーして保持する（参照の無効化対策）
	std::vector<int> childrenCopy = it->second.children;

	// 2. 子供を先に再帰的に削除（ボトムアップ）
	for (auto childId : childrenCopy)
	{
		UnregisterIdLookUpTable(childId);
	}

	// 3. 自分自身をテーブルから削除
	m_collisionIdLookupTable.erase(dataID);

	// 4. ルートリストからも削除
	m_rootCollisionDataId.erase(
		std::remove(m_rootCollisionDataId.begin(), m_rootCollisionDataId.end(), dataID),
		m_rootCollisionDataId.end()
	);
}


void CollisionManager::CreateThreadCollisionObjectProxy(std::vector<ThreadCollisionObjectProxy>* collisionObjectProxy)
{
		std::lock_guard<std::mutex> lock(m_mutex);
	std::vector<ThreadCollisionObjectProxy> localProxies;

	{
		// 読み取りを開始する前にロックを取得！
		localProxies.reserve(m_rootCollisionDataId.size());

		for (auto& id : m_rootCollisionDataId) {
			auto it = m_collisionIdLookupTable.find(id);
			if (it == m_collisionIdLookupTable.end()) continue;

			ThreadCollisionObjectProxy proxy;
			// CreateProxy内でも find を使うため、このスコープ内で一気に処理する
			if (CreateProxy(&proxy, it->second)) {
				localProxies.emplace_back(std::move(proxy));
			}
		}

		// そのまま共有データへ代入
		*collisionObjectProxy = std::move(localProxies);
	} // ここでアンロック
}

bool CollisionManager::CreateProxy(ThreadCollisionObjectProxy* pProxy, const CollisionData& collisionData)
{
	// オブジェクトが死んでいたら失敗
	if (collisionData.pGameObject	== nullptr) return false;
	if (collisionData.pCollider		== nullptr) return false;

	pProxy->id = collisionData.id;
	pProxy->tag = collisionData.pGameObject->GetTag();
	pProxy->collider = collisionData.pCollider->GetClone(); // moveすると元のデータが壊れるので注意！
	pProxy->tagBitIndex = collisionData.tagBitIndex;
	pProxy->isActive = collisionData.pGameObject->IsActive();

	pProxy->children.reserve(collisionData.children.size());

	for (auto& childId : collisionData.children)
	{
		// 子供の ID がテーブルに存在するかチェック
		auto it = m_collisionIdLookupTable.find(childId);
		if (it == m_collisionIdLookupTable.end()) continue;

		ThreadCollisionObjectProxy childProxy;
		if (CreateProxy(&childProxy, it->second))
		{
			pProxy->children.emplace_back(std::move(childProxy));
		}
	}
	return true;
}





