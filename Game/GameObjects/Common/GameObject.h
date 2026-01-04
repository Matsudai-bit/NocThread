/*****************************************************************//**
 * @file    GameObject.h
 * @brief   ゲームオブジェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/03/22
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <typeindex>
#include <unordered_map>
#include "Game/Common/Collision/ICollision.h"
#include "Game/Common/EventSystem/EventSystem.h"
#include "Game/GameObjects/Common/GameObjectEvent/GameObjectEvent.h"
#include "Game/Common/EntityManager/IEntity.h"

#include "Game/Common/TaskManager/TaskManager.h"
#include "Game/GameObjects/Common/Transform/Transform.h"


// クラスの前方宣言 ===================================================
class ICollider;		// コライダー
class CommonResources;	// 共通リソース
class CollisionInfo;	// 衝突情報

/**
 * @brief ゲームオブジェクトのタグ
 */
enum class GameObjectTag :	uint32_t
{
	PLAYER				= 1 << 0,		// プレイヤー
	FLOOR				= 1 << 1,	// 床
	WIRE				= 1 << 2,	// ワイヤー
	WALL				= 1 << 3,	// 壁
	BALL				= 1 << 4,	// ボール
	STAGE_OBJECT		= 1 << 5,	// ステージ上のオブジェクト
	ENEMY				= 1 << 6,	// 敵
	TREASURE			= 1 << 7,	// 宝
	BUILDING			= 1 << 8,	// 建物
	ESCAPE_HELICOPTER	= 1 << 9,	// 脱出用ヘリコプター
	WIRE_GRAPPING_AREA	= 1 << 10,	// ワイヤー照準範囲
	CAMERA				= 1 << 11,	// カメラ
	CHECKPOINT			= 1 << 12,	// チェックポイントオブジェクト

};

// ビット演算子を使いやすくするためのオーバーロード（任意）
inline GameObjectTag operator|(GameObjectTag a, GameObjectTag b) 
{
	return static_cast<GameObjectTag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
// AND演算子 (&) の定義 (判定時に使う)
inline GameObjectTag operator&(GameObjectTag a, GameObjectTag b) 
{
	return static_cast<GameObjectTag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}



// ==演算子の定義
inline bool operator==(GameObjectTag a, GameObjectTag b)
{
	return static_cast<uint32_t>(a) == static_cast<uint32_t>(b);
}


// クラスの定義 ===============================================================
/**
 * @brief	ゲームオブジェクト
 */
class GameObject 
	: public ICollision
	, public Task
{
// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float GRAVITY_SCALE = 25.0f;						// 重力量(s)
	static constexpr DirectX::SimpleMath::Vector3 GRAVITY_ACCELERATION =
		DirectX::SimpleMath::Vector3(0.0f, -GRAVITY_SCALE, 0.0f);		// 重力加速度


// データメンバの宣言 -----------------------------------------------
private:

	const CommonResources*				m_pCommonResources;			///< 共通リソース
	EventSystem<GameObjectEventType>	m_gameObjectEventSystem;	///< イベントシステム

	std::unordered_map<std::type_index, void*>  m_childs;

	std::unique_ptr<Transform> m_transform;		///< トランスフォーム
// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GameObject();

	// デストラクタ
	virtual ~GameObject();

// 操作
public:

	// 型を登録する
	template<typename ChildType>
	void RegisterChildType(ChildType* ptr)
	{
		m_childs[typeid(ChildType)] = ptr;

	}

	// 指定した型にキャストする
	template<typename CastType>
	CastType* CastTo() const
	{
		auto it = m_childs.find(typeid(CastType));
		return (it != m_childs.end()) ? static_cast<CastType*>(it->second) : nullptr;
	}


	// 衝突判定をする直前に行う処理
	void PreCollision() override {} ;

	// 衝突判定をした直後に行う処理
	void PostCollision() override {};
	
	// 衝突処理
	void OnCollision(const CollisionInfo& info) override { UNREFERENCED_PARAMETER(info); }


// 取得/設定
public:


	// 活動しているかどうか
	virtual bool IsActive() const = 0;

	// ゲームオブジェクトタグの取得
	virtual GameObjectTag GetTag() const = 0;

	// トランスフォームの取得
	Transform* GetTransform() const { return m_transform.get(); }

	// 共通リソースの取得
	const CommonResources* GetCommonResources() const {	return m_pCommonResources;}
	// 共通リソースの設定
	void SetCommonResources(const CommonResources* pCommonResources) { m_pCommonResources = pCommonResources; }


// 内部実装
private:

	// イベントのヘルパーメソッド
public:


	// イベントIDのデフォルト処理優先度を設定する
	void SetEventDefaultPriority(GameObjectEventType id, int priority)
	{
		m_gameObjectEventSystem.SetEventDefaultPriority(id, priority);
	}
	// リスナー登録: イベントIDと、対応するEventData型、コールバックを受け取る
	template<typename TEventData, typename Func>
	void AddEventListener(GameObjectEventType id, Func callback, int listenerPriority = 0) // リスナー優先度
	{
		m_gameObjectEventSystem.AddListener < TEventData, Func>(id, callback, listenerPriority);
	}
	// 特定のイベントIDに登録された全てのリスナーを削除する
	void RemoveEventListener(GameObjectEventType id)
	{
		m_gameObjectEventSystem.RemoveListener(id);
	}
	// 全てのイベントIDに登録されたリスナーを全て削除する
	void RemoveAllEventListener()
	{
		m_gameObjectEventSystem.RemoveAllListeners();
	}

	// イベントキューから指定されたイベントIDのイベントを全て削除する（処理はしない）
	void RemoveQueuedEvents(GameObjectEventType id)
	{
		m_gameObjectEventSystem.RemoveQueuedEvents(id);
	}
	// イベントキューのイベントを全て削除する（処理はしない）
	void RemoveAllQueuedEvents()
	{
		m_gameObjectEventSystem.RemoveAllQueueEvents();
	}

	void FireEvent(GameObjectEventType id, std::unique_ptr<EventData> data)
	{
		m_gameObjectEventSystem.FireEvent(id, std::move(data));
	}

	// イベントをまとめて実行
	void ApplyEvents()
	{
		m_gameObjectEventSystem.ApplyEvents();
	}
	// イベントの部分実行
	void ApplyEvent(GameObjectEventType id)
	{
		m_gameObjectEventSystem.ApplyEvent(id);
	}


};
