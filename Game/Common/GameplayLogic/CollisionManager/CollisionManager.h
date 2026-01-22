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

// ヘッダファイルの読み込み ===================================================

// ユーティリティ関連
#include "Game/Common/Collision/Collision.h"
#include "Game/Common/Collision/ICollision.h"

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
	GameObject* pGameObject;
	ICollider* pCollider;

	uint32_t	tagBitIndex;

	std::vector<CollisionData> children;

	CollisionData()
		: pGameObject	{ nullptr }
		, pCollider		{ nullptr }
		, tagBitIndex	{ 0 }
		, children{}
	{}

	CollisionData(GameObject* pGameObject, ICollider* pCollider, std::vector<CollisionData> children = std::vector<CollisionData>())
		: CollisionData{}
	{
		this->pGameObject	= pGameObject;
		this->pCollider		= pCollider;
		this->children		= children;
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
		const CollisionData* pCollisionDataA;
		const CollisionData* pCollisionDataB;
	};

	// エイリアス宣言
public:
	
	// データメンバの宣言 -----------------------------------------------
private:

	// 衝突判定リスト
	std::vector<CollisionData>	m_collisionData;
	std::vector<BroadCollision> m_broadCollisionData;

	const CollisionMatrix* m_pCollisionMatrix;

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
	void AddCollisionData(const CollisionData& collisionData, ICollider* parent);
	// 削除
	void RemoveCollisionObjectData(GameObject* pAddGameObject, ICollider* pCollider);

	// 何かのオブジェクトと当たったかどうか
	bool IsCollisionWithAnyObject(const ICollider& checkCollider);

	// 衝突判定
	static bool DetectCollision(const ICollider* pColliderA, const ICollider* pColliderB);

	// 指定したオブジェクトの種類と衝突しているかどうか
	bool IsCollidingWithObjectType(const ICollider* pCheckCollider, const GameObjectTag& checkTag);

	// 指定したコライダーの衝突情報を取得する
	bool RetrieveCollisionData(const ICollider* pCheckCollider, std::vector<const GameObject*>* pHitGameObjects, std::vector<const ICollider*>* pHitColliders = nullptr);


	// 取得/設定
public:

	// 衝突検知表の設定
	void SetCollisionMatrix(const CollisionMatrix* pCollisionMatrix) {m_pCollisionMatrix = pCollisionMatrix;}

	// 内部実装
private:

	// 衝突判定直前の処理
	void PreCollision();
	// 衝突の検知
	void UpdateDetection(std::vector<DetectedCollisonData>* pOutResults);

	// 衝突の通知
	void NotifyCollisionEvents(std::vector<DetectedCollisonData>* pDetectedCollisions);

	// 事後処理
	void FinalizeCollision();


	// ペアの衝突チェック
	void CheckCollisionPair(const CollisionData& collisionDataA, const CollisionData& collisionDataB, std::vector<DetectedCollisonData>* pOutResults);

	
};
