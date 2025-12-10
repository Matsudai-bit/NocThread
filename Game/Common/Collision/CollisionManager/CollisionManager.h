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
#include "Game/Common/Collision/Collision.h"
#include "Game/Common/Collision/ICollision.h"
#include "Game/GameObjects/Common/GameObject.h"

#include "Game/Common/TaskManager/TaskManager.h"

// クラスの定義 ===============================================================
/**
  * @brief 当たり判定管理
  */


  // 前置宣言
class GameObject;

/**
 * @brief 衝突情報
 */
struct CollisionInfo
{
	ICollider*	otherCollider;	// 相手のコライダー
	GameObject* otherObject;	// 相手のオブジェクト
	ICollider*	myCollider;		// 自分の衝突コライダー
};



class CollisionManager
	: public Task
{


	// クラス定数の宣言 -------------------------------------------------
public:

	// 構造体の宣言
private:

	/**
	 * @brief 衝突判定に必要なデータ
	 */
	struct CollisionData
	{
		GameObject* pGameObject;
		ICollider* pCollider;
	};


	/**
	 * @brief 衝突したオブジェクトの組み合わせ
	 */
	struct DetectedCollisonData
	{
		CollisionData* pCollisionDataA;
		CollisionData* pCollisionDataB;
	};

	// エイリアス宣言
public:
	// 衝突判定の情報







	// データメンバの宣言 -----------------------------------------------
private:

	// 衝突判定リスト
	std::vector<CollisionData> m_collisionData;

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
	void AddCollisionObjectData(GameObject* pAddGameObject, ICollider* pCollider);
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


	// 内部実装
private:







};
