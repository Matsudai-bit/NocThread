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
#include "CollisionDetectionWorker/CollisionDetectionWorker.h"
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

#ifdef COLLISIONMANAGER_DEBUG	
	, m_totalDuration{}
#endif 

{
	 m_proxy.resize(1336);

	 m_collisionDetectionWorker = std::make_unique<CollisionDetectionWorker>();

	//m_detectionThread = std::make_unique<std::thread>(&CollisionManager::UpdateDetection, this, &m_detectionResults);

}



/**
 * @brief デストラクタ
 */
CollisionManager::~CollisionManager()
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

#ifdef COLLISIONMANAGER_DEBUG	
	// 1. 開始時刻の記録
	auto start = std::chrono::high_resolution_clock::now();
	OutputDebugString(L"============ 衝突管理の更新処理 ============\n");
# endif 
	
	m_collisionDetectionWorker->WaitForEndCalculation();
	auto detectionResults = std::move(m_collisionDetectionWorker->GetDetectionResults());
	
	// 衝突を通知する
	StaticNotifyCollisionEvents(&detectionResults);

	// 事後処理
	FinalizeCollision();

#ifdef COLLISIONMANAGER_DEBUG	
	//// 3. 終了時刻の記録
	auto end = std::chrono::high_resolution_clock::now();

	// 4. 処理時間の計算と表示
	//duration_cast で希望の単位に変換（ここではマイクロ秒 ?s）
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	wchar_t buffer[64];
	swprintf_s(buffer, L"duration : %lld us\n", duration.count());

	// 3. Send to the Output window
	OutputDebugString(buffer);

	OutputDebugString(L"============ 衝突管理の終了 ============\n");
# endif 



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

	if (m_detectionThread)
	{
		m_stopThread = true;
		m_cv.notify_one();
		m_detectionThread->join();
	}


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
				return  m_collisionIdLookupTable[static_cast<size_t>(id)].pCollider == parent;
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
	//RemoveAll();
	if (!pAddGameObject || !pCollider) return;

	// ここで一回だけロック！
	std::lock_guard<std::mutex> lock{ m_mutex };

	auto findIt = std::find_if(m_rootCollisionDataId.begin(), m_rootCollisionDataId.end(),
		[&](const UINT& id) {
			auto it = m_collisionIdLookupTable.find(id);
			if (it == m_collisionIdLookupTable.end()) return false;
			return (pAddGameObject == it->second.pGameObject && pCollider == it->second.pCollider);
		});

	if (findIt != m_rootCollisionDataId.end()) {
		UINT idToRemove = *findIt;

		// 内部用関数を呼ぶ（再帰の中で二重ロックしないように）
		UnregisterIdLookUpTable(idToRemove);

		// IDのリサイクル（※本来は遅延返却が望ましい）
		m_idPool.RecycleID(idToRemove);
	}
}

void CollisionManager::RemoveAll()
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	m_collisionIdLookupTable.clear();
	m_rootCollisionDataId.clear();

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
 * @brief 指定したコライダーの衝突情報を取得
 *
 * @param[in] pCheckCollider	調べるコライダー
 * @param[in] pHitGameObjects	衝突データ
 *
 * @return true 衝突しているかどうか
 */
bool CollisionManager::RetrieveCollisionData(const ICollider* pCheckCollider, std::vector<const GameObject*>* pHitGameObjects, std::vector<const ICollider*>* pHitColliders)
{
	if (!pCheckCollider) return false;

	bool isHit = false;

	// テーブル参照中の変更を防ぐためロックを取得
	std::lock_guard<std::mutex> lock(m_mutex);

	// 再帰的に衝突を調べるラムダ関数
	auto retrieveRecursive = [&](auto& self, const CollisionData& currentData) -> void {
		// オブジェクトの有効性・活動状態のチェック
		if (!currentData.pGameObject || !currentData.pCollider) return;
		if (!currentData.pGameObject->IsActive()) return;

		// 衝突判定を実行
		if (DetectCollision(currentData.pCollider, pCheckCollider))
		{
			isHit = true;

			// ポインタが渡されている場合のみ、情報をリストに追加
			if (pHitGameObjects)
			{
				pHitGameObjects->push_back(currentData.pGameObject);
			}
			if (pHitColliders)
			{
				pHitColliders->push_back(currentData.pCollider);
			}
		}

		// 子オブジェクトに対しても同様に判定を行う
		for (const auto& childId : currentData.children)
		{
			auto itChild = m_collisionIdLookupTable.find(childId);
			if (itChild != m_collisionIdLookupTable.end())
			{
				self(self, itChild->second);
			}
		}
		};

	// ルートリストに登録されているすべてのオブジェクトから走査開始
	for (const auto& id : m_rootCollisionDataId)
	{
		auto it = m_collisionIdLookupTable.find(id);
		if (it != m_collisionIdLookupTable.end())
		{
			retrieveRecursive(retrieveRecursive, it->second);
		}
	}

	return isHit;
}

void CollisionManager::StartThread()
{

#ifdef COLLISIONMANAGER_DEBUG	
	OutputDebugString(L"============ スレッドの開始処理 ============\n");
#endif 

	// 衝突判定の流れ
// 衝突判定->衝突処理->押し出し
	if (m_rootCollisionDataId.empty()) return ;

	// 1. 開始時刻の記録
	auto start = std::chrono::high_resolution_clock::now();


	// 事前処理
	PreCollision();

	// 判定用データの作成（ここはメインスレッドで安全に行う）
	auto nextProxy = std::make_unique<std::vector<ThreadCollisionObjectProxy>>();
	CreateThreadCollisionObjectProxy(nextProxy.get());

#ifdef COLLISIONMANAGER_DEBUG	

	//// 3. 終了時刻の記録
	auto end = std::chrono::high_resolution_clock::now();

	// 4. 処理時間の計算と表示
	//duration_cast で希望の単位に変換（ここではマイクロ秒 ?s）
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	m_totalDuration = 0;
	m_totalDuration += duration.count();

#endif 

	// 静的オブジェクトをコピー
	nextProxy->insert(nextProxy->end(), m_staticProxies.begin(), m_staticProxies.end());

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// 2. 「コピー」ではなく「移動代入」を使う
		// これにより、m_proxy の中身が一瞬で nextProxy と入れ替わります ($O(1)$)
		m_collisionDetectionWorker->StartAsync(std::move(nextProxy));
		m_detectionResults.clear();
		m_applyThread = true;

#ifdef COLLISIONMANAGER_DEBUG	
		OutputDebugString(L"============ スレッドの開始要求 ============\n");
#endif 
	}
	m_cv.notify_one(); // サブスレッドを起こす
}

void CollisionManager::PreCreateProxy()
{
	m_staticProxies.clear();
	CreateStaticProxy(&m_staticProxies);
}

void CollisionManager::SetCollisionMatrix(const CollisionMatrix* pCollisionMatrix)
{
	m_collisionDetectionWorker->SetCollisionMatrix(*pCollisionMatrix);
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
void CollisionManager::UpdateDetection(std::vector<DetectedCollisonData>* pOutResults)
{
	// サブスレッド内だけで使う「完全なローカル」スタック
	std::vector<DetectedCollisonData> localBuffer;
	localBuffer.reserve(256); // あらかじめある程度確保しておく
	while (!m_stopThread)
	{
		std::vector<ThreadCollisionObjectProxy> localProxy;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            // 「仕事が来る(m_applyThread)」か「終了する(m_stopThread)」までスリープ
            m_cv.wait(lock, [this] { return m_applyThread || m_stopThread; });

#ifdef COLLISIONMANAGER_DEBUG	
			OutputDebugString(L"============ 仕事を受け取った ============\n");
#endif 

            if (m_stopThread) break;

            // 判定に使うデータをローカルにコピー（メインスレッドとの干渉を防ぐ）
			localProxy = std::move(m_proxy);
			m_isCalculating = true;
            m_applyThread = false; // 受付完了
        }

		if (localProxy.size() >= 2)
		{
			for (size_t i = 0; i < localProxy.size() - 1; i++)
			{
				for (size_t j = i + 1; j < localProxy.size(); j++)
				{
					// 1. ロックを一切気にせず、ローカルバッファにどんどん溜める
					CheckCollisionPair(localProxy[i], localProxy[j], &localBuffer);

					// 2. バッファが一定数ったら一気に共有メモリへ
					if (localBuffer.size() >= 50)
					{
#ifdef COLLISIONMANAGER_DEBUG	
						OutputDebugString(L"=========== データ更新待機 ========== \n");
#endif 

						std::lock_guard<std::mutex> lock(m_mutex);
						// std::copy よりも insert が効率的
						pOutResults->insert(pOutResults->end(), localBuffer.begin(), localBuffer.end());
						localBuffer.clear();

#ifdef COLLISIONMANAGER_DEBUG	
						OutputDebugString(L"!!!! 衝突データの更新 !!!! \n");
#endif 

						m_notifyCV.notify_one();
					}
				}
			}
		}

		// 3. ループ終了後に、残っているデータをすべて転送
		if (!localBuffer.empty())
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			pOutResults->insert(pOutResults->end(), localBuffer.begin(), localBuffer.end());
			localBuffer.clear();
		}

		m_isCalculating = false;

#ifdef COLLISIONMANAGER_DEBUG	
		OutputDebugString(L"!!!! 衝突処理の終了 !!!! \n");
#endif 
		m_notifyCV.notify_one(); // 完了通知
	}
}

/**
 * @brief 衝突の通知
 *
 * @param[in] pDetectedCollisions 検知された衝突
 */
void CollisionManager::DynamicNotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions)
{
	// 一時的にデータを溜めるローカルバッファ
	std::vector<DetectedCollisonData> localWorkList;

	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);

#ifdef COLLISIONMANAGER_DEBUG	
			OutputDebugString(L"=========== 衝突データがくるまで待機 ========== \n");
#endif 

			// データが空かつ計算中なら待機
			m_notifyCV.wait(lock, [this, pDetectedCollisions] {
				return !pDetectedCollisions->empty() || !m_isCalculating;
				});

			// 終了判定
			if (pDetectedCollisions->empty() && !m_isCalculating) {
				break;
			}

			// 【高速化の肝】個別に pop_back せず、中身を丸ごと move する
			// これにより、ロックを保持する時間を一瞬にする
			localWorkList = std::move(*pDetectedCollisions);
			pDetectedCollisions->clear();
		} // ここでロックが解除される

#ifdef COLLISIONMANAGER_DEBUG	
		OutputDebugString(L"=========== 衝突通知 ========== \n");
#endif 


		// --- ここからロックの外 ---
		// まとめて取得したデータを一気に処理
		for (const auto& data : localWorkList)
		{
			// IDから実体を取得 (ここは読み取り専用なら lock 外でも OK。
			// ただしテーブルが他スレッドで書き換わるなら lock が必要)
			// 安全策をとるなら、ここだけ最小限の lock をかける

			auto itA = m_collisionIdLookupTable.find(data.collisionDataIdA);
			auto itB = m_collisionIdLookupTable.find(data.collisionDataIdB);

			if (itA != m_collisionIdLookupTable.end() && itB != m_collisionIdLookupTable.end())
			{
				CollisionData& collA = itA->second;
				CollisionData& collB = itB->second;

				if (collA.pGameObject && collB.pGameObject)
				{
					// ユーザー定義の重い処理を呼び出し
					collA.pGameObject->OnCollision(CollisionInfo(collB.pCollider, collB.pGameObject, collA.pCollider));
					collB.pGameObject->OnCollision(CollisionInfo(collA.pCollider, collA.pGameObject, collB.pCollider));
				}
			}
		}

		localWorkList.clear(); // 次のバッチのためにクリア
	}


#ifdef COLLISIONMANAGER_DEBUG	
	OutputDebugString(L"=========== 衝突通知終了 ========== \n");
#endif 

}

void CollisionManager::StaticNotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions)
{
	// ここにはもうロックも wait も不要。純粋な通知ロジックのみ。
	for (const auto& data : (*pDetectedCollisions))
	{
		auto itA = m_collisionIdLookupTable.find(data.collisionDataIdA);
		auto itB = m_collisionIdLookupTable.find(data.collisionDataIdB);

		if (itA != m_collisionIdLookupTable.end() && itB != m_collisionIdLookupTable.end())
		{
			CollisionData& collA = itA->second;
			CollisionData& collB = itB->second;

			if (collA.pGameObject && collB.pGameObject)
			{
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
void CollisionManager::CheckCollisionPair(const ThreadCollisionObjectProxy& collisionDataA, const ThreadCollisionObjectProxy& collisionDataB, std::vector<DetectedCollisonData>* pOutResultStack )
{
	if (collisionDataA.id == collisionDataB.id) return;

	
	// 活動していなければ飛ばす
	if (collisionDataA.isActive == false) return;
	//活動していなければ飛ばす
	if (collisionDataB.isActive == false) return;



	// 衝突検知をするかどうか
	if (!CanDetect(collisionDataA, collisionDataB)) 
	{
		return;
	}

	// 衝突しているかどうか
	if (DetectCollision(collisionDataA.collider.get(), collisionDataB.collider.get()))
	{	
		pOutResultStack->push_back({ collisionDataA.id, collisionDataB.id });

		// 子を持っている場合その子も検知する
		for (auto& child : collisionDataA.children)
		{
			CheckCollisionPair( child, collisionDataB, pOutResultStack);
		}
		for (auto& child : collisionDataB.children)
		{
			CheckCollisionPair(child, collisionDataA, pOutResultStack);
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

void CollisionManager::UnregisterIdLookUpTable(UINT dataID)
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
	std::vector<ThreadCollisionObjectProxy> localProxies;

#ifdef COLLISIONMANAGER_DEBUG	
	OutputDebugString(L"============ 動的プロキシの作成処理 ============\n");
#endif 
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		// 読み取りを開始する前にロックを取得！
		localProxies.reserve(m_rootCollisionDataId.size());

		for (auto& id : m_rootCollisionDataId) {
			auto it = m_collisionIdLookupTable.find(id);
			if (it == m_collisionIdLookupTable.end()) continue;

			ThreadCollisionObjectProxy proxy;
			// CreateProxy内でも find を使うため、このスコープ内で一気に処理する
			if (CreateProxy(&proxy, it->second, false)) {
				localProxies.emplace_back(std::move(proxy));
			}
		}

		// そのまま共有データへ代入
		*collisionObjectProxy = std::move(localProxies);
	} // ここでアンロック
}

bool CollisionManager::CreateProxy(ThreadCollisionObjectProxy* pProxy, const CollisionData& collisionData, bool isStaticCreation)
{
	if (collisionData.isStatic != isStaticCreation) { return false; }

	// オブジェクトが死んでいたら失敗
	if (collisionData.pCollider		== nullptr) return false;


	pProxy->id			= collisionData.id;
	pProxy->collider	= collisionData.pCollider->GetClone(); // moveすると元のデータが壊れるので注意！
	pProxy->tagBitIndex = collisionData.tagBitIndex;

	pProxy->tag			= (collisionData.pGameObject) ? collisionData.pGameObject->GetTag() : GameObjectTag::DEFAULT;
	pProxy->isActive	= (collisionData.pGameObject) ? collisionData.pGameObject->IsActive() : true;

	pProxy->children.reserve(collisionData.children.size());



	for (auto& childId : collisionData.children)
	{
		// 子供の ID がテーブルに存在するかチェック
		auto it = m_collisionIdLookupTable.find(childId);
		if (it == m_collisionIdLookupTable.end()) continue;

		ThreadCollisionObjectProxy childProxy;
		if (CreateProxy(&childProxy, it->second, isStaticCreation))
		{
			pProxy->children.emplace_back(std::move(childProxy));
		}
	}
	return true;
}

bool CollisionManager::CreateStaticProxy(std::vector<ThreadCollisionObjectProxy>* collisionObjectProxy)
{
	std::vector<ThreadCollisionObjectProxy> localProxies;

#ifdef COLLISIONMANAGER_DEBUG	
	OutputDebugString(L"============ 静的プロキシの作成処理 ============\n");
#endif 
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		// 読み取りを開始する前にロックを取得！
		localProxies.reserve(m_rootCollisionDataId.size());

		for (auto& id : m_rootCollisionDataId) {
			auto it = m_collisionIdLookupTable.find(id);
			if (it == m_collisionIdLookupTable.end()) continue;

			ThreadCollisionObjectProxy proxy;
			// CreateProxy内でも find を使うため、このスコープ内で一気に処理する
			if (CreateProxy(&proxy, it->second, true)) {
				localProxies.emplace_back(std::move(proxy));
			}
		}

		// そのまま共有データへ代入
		*collisionObjectProxy = std::move(localProxies);
	} // ここでアンロック}
	return true;
}

/**
 * @brief 衝突検知できるかどうか
 * 
 * @param[in] collisionDataA　衝突データA
 * @param[in] collisionDataB　衝突データB
 * 
 * @returns true 可能
 * @returns false 不可能
 */
bool CollisionManager::CanDetect(
	 const ThreadCollisionObjectProxy& collisionDataA,
	 const ThreadCollisionObjectProxy& collisionDataB)
{
	// ゲームオブジェクトタグのビットインデックス
	uint32_t gameObjectTagIndexA = collisionDataA.tagBitIndex;
	// 衝突検知をするかどうか
	return m_pCollisionMatrix && m_pCollisionMatrix->ShouldCollide(gameObjectTagIndexA, collisionDataB.tag);
}





