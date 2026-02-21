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
#include "../CollisionDispatcher/CollisionDispatcher.h"
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


#ifdef COLLISIONMANAGER_DEBUG	
	, m_totalDuration{}
#endif 

{

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

	// 事前処理
	PreCollision();
	
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

}


void CollisionManager::AddCollisionData(const CollisionData& collisionData)
{

	auto newId = RegisterIdLookUpTable(collisionData);

	m_rootCollisionDataId.push_back(newId);

}

void CollisionManager::AddCollisionData(const CollisionData& childCollisionData, ICollider* parent)
{
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
	m_collisionIdLookupTable.clear();
	m_rootCollisionDataId.clear();

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

	// 再帰的に衝突を調べるラムダ関数
	auto retrieveRecursive = [&](auto& self, const CollisionData& currentData) -> void {
		// オブジェクトの有効性・活動状態のチェック
		if (!currentData.pGameObject || !currentData.pCollider) return;
		if (!currentData.pGameObject->IsActive()) return;

		// 衝突判定を実行
		if (CollisionDispatcher::DetectCollision(currentData.pCollider, pCheckCollider))
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
	m_collisionDetectionWorker->StartAsync(std::move(nextProxy));
	

#ifdef COLLISIONMANAGER_DEBUG	
		OutputDebugString(L"============ スレッドの開始要求 ============\n");
#endif 
	
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
	 // ここでアンロック
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
	// ここでアンロック}
	return true;
}







