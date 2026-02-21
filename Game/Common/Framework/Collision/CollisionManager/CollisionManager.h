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

//#define COLLISIONMANAGER_DEBUG

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
#include "CollisionTypes/CollisionTypes.h"

class CollisionDetectionWorker;
class CollisionMatrix;

// クラスの定義 ===============================================================
/**
 * @brief 当たり判定管理
 */
  // 前置宣言
class GameObject;

class CollisionManager
	: public Task
{
	// クラス定数の宣言 -------------------------------------------------
public:

	// 構造体の宣言
private:



	// エイリアス宣言
public:

	// データメンバの宣言 -----------------------------------------------
private:

	// 衝突データ関連
	MyLib::IdPool<UINT> m_idPool;										///< 識別子の作成プール
	std::vector<UINT>	m_rootCollisionDataId;							///< 衝突データのルートデータ群
	std::unordered_map<UINT, CollisionData>	m_collisionIdLookupTable;	///< 早期アクセス用衝突データ群
	const CollisionMatrix* m_pCollisionMatrix; ///< 衝突対応表

	// スレッド関連
	std::unique_ptr<CollisionDetectionWorker> m_collisionDetectionWorker;	///< 衝突検知者
	std::vector<ThreadCollisionObjectProxy> m_staticProxies;				///< 静的なプロキシ（同じ情報のプロキシを作らないようにするため）


#ifdef COLLISIONMANAGER_DEBUG
	long long m_totalDuration;

# endif 


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionManager();

	// デストラクタ
	~CollisionManager();


	// 操作
public:

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	
	// 非同期当たり判定の開始（判定タスクの発注）
	void RequestCollisionDetection();

	// 静的なプロキシの作成
	void CreateStaticProxy();

	// 終了処理
	void Finalize();

	// 衝突判定をするオブジェクトの追加
	void AddCollisionData(const CollisionData& collisionData);
	void AddCollisionData(const CollisionData& childCollisionData, ICollider* parent);
	// 削除
	void RemoveCollisionObjectData(GameObject* pRemoveGameObject, ICollider* pCollider);
	void RemoveAll();



	// 指定したコライダーの衝突情報を取得する
	bool RetrieveCollisionData(const ICollider* pCheckCollider, std::vector<const GameObject*>* pHitGameObjects, std::vector<const ICollider*>* pHitColliders = nullptr);



	// 取得/設定
public:

	// 衝突検知表の設定
	void SetCollisionMatrix(const CollisionMatrix* pCollisionMatrix);

	// 内部実装
private:

	// 同期してリザルトデータを更新
	std::vector<DetectedCollisonData> SyncAndFetchResults();

	// 衝突判定直前の処理
	void PreCollision();

	// 衝突の通知
	void NotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions);

	// 事後処理
	void FinalizeCollision();

	// 早期アクセス用テーブルに登録する
	UINT RegisterIdLookUpTable(CollisionData data);
	// 早期アクセス用テーブルから削除する
	void UnregisterIdLookUpTable(UINT dataID);

	//  ワーカープロキシを作成
	void CreateWorkerProxy(std::vector<ThreadCollisionObjectProxy>* collisionObjectProxy, bool isStaticProxy);
	// プロキシの作成
	bool CreateProxy(ThreadCollisionObjectProxy* pProxy, const CollisionData& collisionData, bool isStaticCreation);
};
