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

// クラスの前方宣言 ===================================================
class ICollider;		// コライダー
class CommonResources;	// 共通リソース

/**
 * @brief ゲームオブジェクトのタグ
 */
enum class GameObjectTag
{
	PLAYER,
	FLOOR,
	WIRE,
	WALL,
	BALL,
	STAGE_OBJECT,
	ENEMY,
	TREASURE,
	BUILDING,				// 建物
	ESCAPE_HELICOPTER,		// 脱出用ヘリコプター
	WIRE_GRAPPING_AREA,		// ワイヤー照準範囲
	CAMERA					// カメラ
};

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

	DirectX::SimpleMath::Vector3	m_position;	///< 座標
	DirectX::SimpleMath::Quaternion m_defaultRotation;	///< デフォルトの回転量
	DirectX::SimpleMath::Quaternion m_rotation;	///< 回転
	float							m_scale;	///< 拡大率

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


	
	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override = 0;



// 取得/設定
public:

	// 活動しているかどうか
	virtual bool IsActive() const = 0;

	// ゲームオブジェクトタグの取得
	virtual GameObjectTag GetTag() const = 0;

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;
	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position);

	// 回転の取得
	DirectX::SimpleMath::Quaternion GetRotate() const;
	// デフォルトの回転の取得
	DirectX::SimpleMath::Quaternion GetDefaultRotate() const;
	// 回転の設定
	void SetRotate(const DirectX::SimpleMath::Quaternion& rotate);
	void SetDefaultRotate(const DirectX::SimpleMath::Quaternion& rotate);

	// 拡大率の取得
	float GetScale() const;
	// 拡大率の設定
	void SetScale(const float& scale);


	// 前方の取得
	DirectX::SimpleMath::Vector3 GetForward() const;


	// 共通リソースの取得
	const CommonResources* GetCommonResources() const;

	// 共通リソースの設定
	void SetCommonResources(const CommonResources* pCommonResources);



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
