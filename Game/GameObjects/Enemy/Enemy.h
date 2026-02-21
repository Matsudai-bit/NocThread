/*****************************************************************//**
 * @file    Enemy.h
 * @brief   敵に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/30
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Enemy/IEnemy.h"

// フレームワーク関連
#include "Game/Common/Framework/Event/WireSystemObserver/IWireEventObserver.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"

// クラスの前方宣言 ===================================================
class CommonResources; // 共通リソース
class CollisionManager;// 衝突管理
class CollisionInfo;  // 衝突情報
class Player;		  // プレイヤー

class IEnemyInitializeBehaviour;// 初期化処理インターフェース
class IEnemyUpdateBehaviour;	// 更新処理インターフェース
class IEnemyDrawBehaviour;		// 描画処理インターフェース

// クラスの定義 ===============================================================
/**
 * @brief 敵　(基底)
 */
class Enemy
	: public MovableObject
	, public IEnemy
	, public IWireEventObserver
{
// 列挙型の宣言　　-------------------------------------------------
public:

	/**
	 * @brief 種類
	 */
	enum class Type
	{
		NORMAL,
	};

// クラス定数の宣言 -------------------------------------------------
public:

// データメンバの宣言 -----------------------------------------------
private:
	std::vector<std::unique_ptr<IEnemyUpdateBehaviour>> m_updateBehaviour;	// 更新処理の挙動群
	
	DirectX::Model m_model;

	std::unique_ptr<Sphere> m_collider;

	bool m_isHold;
	bool m_isThrow;
	bool m_isActive;
	bool m_isGrounded; // 接地している

	float m_deltaTime;

	DirectX::SimpleMath::Vector3 m_overlapTotal;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Enemy();

	// デストラクタ
	~Enemy();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager) override;

	// 更新処理
	void Update(float deltaTime) override;

	// 描画処理
	void Draw(const Camera& camera) override;

	// 終了処理
	void Finalize();

	// 挙動の登録
	void AddUpdateBehaviour(std::unique_ptr<IEnemyUpdateBehaviour> updateBehaviour);
	

// 取得/設定
public:
	// 活動しているかどうか
	bool IsActive() const override { return m_isActive; }
	

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override {	return GameObjectTag::ENEMY; }

// イベント関連
public:

	// 衝突時に呼ばれる
	void OnCollision(const CollisionInfo& info) override;

	void PreCollision() override;

	void PostCollision() override;

	//  ワイヤーに掴まれたときの挙動
	void OnWireGrab(const WireEventData& eventData) override;

	// ワイヤーから離れたとき
	void OnWireRelease(const WireEventData& eventData) override;

	// ワイヤーで引き寄せられるとき
	void OnWirePull(const WireEventData& eventData) override;

	// ワイヤーで投げられるとき
	void OnWireThrow(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse) override;

	// ワイヤーに掴まれている時
	void OnWireHover(const WireEventData& eventData) override;

// 内部実装
private:

	// 建物との衝突判定
	void OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider);

};
