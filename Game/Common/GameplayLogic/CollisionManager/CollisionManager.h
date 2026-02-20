/**
 * @file   CollisionManager.h
 *
 * @brief  当たり判定管理に関するヘッダファイル
 *
 * @author 松下大暉
 *
 * @date   2025/03/22
 */

 // 多重インクルードの防止 =====================================================
#pragma once


#include <vector>
#include <thread>
#include <mutex>

// ヘッダファイルの読み込み ===================================================

// ライブラリ関連 
#include "Library/MyLib/IdPool/IdPool.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"
#include "Game/Common/Utillities/Collision/ICollision.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/GameObject.h"

// フレームワーク関連
#include "Game/Common/Framework/TaskManager/TaskManager.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/CollisionMatrix/CollisionMatrix.h"

// クラスの定義 ===============================================================
/**
 * @brief 当たり判定管理
 */
  // 前置宣言
class GameObject;
/**
 * @brief 衝突情報
 */
class CollisionInfo
{

public:
	ICollider*	pOtherCollider;	// 相手のコライダー
	GameObject* pOtherObject;	// 相手のオブジェクト
	ICollider*	pMyCollider;		// 自分の衝突コライダー

	CollisionInfo(ICollider* pOtherCollider, GameObject* pOtherObject, ICollider* pMyCollider)
		: pOtherCollider{ pOtherCollider }
		, pOtherObject	{ pOtherObject }
		, pMyCollider	{ pMyCollider }
	{}
};

struct BroadCollision
{
	ICollider*	pCollider;
	bool		isHit;

	BroadCollision(ICollider* pCollider)
		: pCollider{ pCollider }
		, isHit{ false }
	{

	}
};

/**
 * @brief 衝突判定に必要なデータ
 */
struct CollisionData
{
	GameObject* pGameObject;	///< ゲームオブジェクト
	ICollider* pCollider;		///< コライダー

	int			id;			///< 衝突データ固有のID
	uint32_t	tagBitIndex;	///< 衝突マトリックス時に使用（毎フレーム計算すると重いためのキャッシュ）

	std::vector<int> children;

	bool isStatic; ///< 静的かどうか

	CollisionData()
		: pGameObject	{ nullptr }
		, pCollider		{ nullptr }
		, tagBitIndex	{ 0 }
		, children		{}
		, id			{ -1 }
		, isStatic		{ false }
	{}

	CollisionData(GameObject* pGameObject, ICollider* pCollider, bool isStatic)
		: CollisionData{}
	{
		this->isStatic = isStatic;
		this->pGameObject	= pGameObject;
		this->pCollider		= pCollider;
		if (pGameObject)
		{
			unsigned long index;
			if (_BitScanForward(&index, static_cast<uint32_t>(pGameObject->GetTag())))
			{
				this->tagBitIndex = index;
			}
		}
	}
	

};


class CollisionManager
	: public Task
{
	// クラス定数の宣言 -------------------------------------------------
public:

	// 構造体の宣言
private:
	/**
	 * @brief 衝突したオブジェクトの組み合わせ
	 */
	struct DetectedCollisonData
	{
		int collisionDataIdA;
		int collisionDataIdB;

	/*	DetectedCollisonData()
			: collisionDataIdA{ -1 }
			, collisionDataIdB{ -1 }
		{}*/

	};

	struct ThreadCollisionObjectProxy
	{
		bool isActive;
		GameObjectTag tag;
		uint32_t	tagBitIndex;

		int id;
		std::unique_ptr<ICollider> collider;
		std::vector<ThreadCollisionObjectProxy> children;

		ThreadCollisionObjectProxy()
			: id{-1}
			, tag{ GameObjectTag::DEFAULT }
			, tagBitIndex{ }
			, isActive{ false }

		{ }

		ThreadCollisionObjectProxy(const ThreadCollisionObjectProxy& origin)
			: id{ origin.id }
			, collider{ }
			, children{ origin.children }
			, tag{ origin.tag }
			, tagBitIndex{origin.tagBitIndex }
			, isActive{origin.isActive}
		{
			if (origin.collider)
			{
				collider = origin.collider->GetClone();
			}
		}
		ThreadCollisionObjectProxy& operator=(const ThreadCollisionObjectProxy& origin)
		{
			if (this != &origin) {
				id = origin.id;
				tag = origin.tag;
				tagBitIndex = origin.tagBitIndex;
				isActive = origin.isActive;
				children = origin.children;
				if (origin.collider) {
					collider = origin.collider->GetClone();
				}
				else {
					collider.reset();
				}
			}
			return *this;
		}
	};


	// エイリアス宣言
public:
	
	// データメンバの宣言 -----------------------------------------------
private:

	// 衝突判定リスト
	std::vector<UINT>	m_rootCollisionDataId;
	std::unordered_map<UINT, CollisionData>	m_collisionIdLookupTable; ///< 早期アクセス用衝突データ群

	std::vector<BroadCollision> m_broadCollisionData;

	const CollisionMatrix* m_pCollisionMatrix;

	MyLib::IdPool<UINT> m_idPool;

	std::vector<ThreadCollisionObjectProxy> m_proxy;

	std::mutex m_mutex;
	std::unique_ptr< std::thread> m_detectionThread;
	std::vector<DetectedCollisonData> m_detectionResults;

	std::atomic<bool>	m_isCalculating = true;
	std::atomic<bool>	m_stopThread = false;

	std::atomic<bool> m_applyThread;

	std::condition_variable m_cv;
	std::condition_variable m_notifyCV;

	std::vector<ThreadCollisionObjectProxy> m_staticProxies;

	long long m_totalDuration;

	int m_debugCount[2];
	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionManager();

	// デストラクタ
	~CollisionManager();


	// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();

	// 衝突判定をするオブジェクトの追加
	//void AddCollisionObjectData(GameObject* pAddGameObject, ICollider* pCollider, ICollider* pBroadCollider = nullptr);
	void AddCollisionData(const CollisionData& collisionData);
	void AddCollisionData(const CollisionData& childCollisionData, ICollider* parent);
	// 削除
	void RemoveCollisionObjectData(GameObject* pAddGameObject, ICollider* pCollider);
	void RemoveAll();

	// 何かのオブジェクトと当たったかどうか
	bool IsCollisionWithAnyObject(const ICollider& checkCollider);

	// 衝突判定
	static bool DetectCollision(const ICollider* pColliderA, const ICollider* pColliderB);

	// 指定したオブジェクトの種類と衝突しているかどうか
	bool IsCollidingWithObjectType(const ICollider* pCheckCollider, const GameObjectTag& checkTag);

	// 指定したコライダーの衝突情報を取得する
	bool RetrieveCollisionData(const ICollider* pCheckCollider, std::vector<const GameObject*>* pHitGameObjects, std::vector<const ICollider*>* pHitColliders = nullptr);

	void StartThread();

	// 事前にプロキシを作る
	void PreCreateProxy();

	// 取得/設定
public:

	// 衝突検知表の設定
	void SetCollisionMatrix(const CollisionMatrix* pCollisionMatrix) {m_pCollisionMatrix = pCollisionMatrix;}

	// 内部実装
private:

	// 衝突判定直前の処理
	void PreCollision();
	// 衝突の検知
	void UpdateDetection(const std::vector<ThreadCollisionObjectProxy>* proxy, std::vector<DetectedCollisonData>* pOutResults);

	// 衝突の通知
	void NotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions);

	// 事後処理
	void FinalizeCollision();


	// ペアの衝突チェック
	void CheckCollisionPair(const ThreadCollisionObjectProxy& collisionDataA, const ThreadCollisionObjectProxy& collisionDataB, std::vector<DetectedCollisonData>* pOutResults);

	// 早期アクセス用テーブルに登録する
	UINT RegisterIdLookUpTable(CollisionData data);
	// 早期アクセス用テーブルから削除する
	void UnregisterIdLookUpTable(int dataID);


	void CreateThreadCollisionObjectProxy(std::vector<ThreadCollisionObjectProxy>* collisionObjectProxy);
	bool CreateProxy(ThreadCollisionObjectProxy* pProxy, const CollisionData& collisionData, bool isStaticCreation);
	bool CreateStaticProxy(std::vector<ThreadCollisionObjectProxy>* collisionObjectProxy);

	// 検知できるかどうか
	bool CanDetect(const ThreadCollisionObjectProxy& collisionDataA, const ThreadCollisionObjectProxy& collisionDataB);
};
