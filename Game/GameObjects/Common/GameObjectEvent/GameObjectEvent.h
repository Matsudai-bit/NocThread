/*****************************************************************//**
 * @file    GameObjectEvent.h
 * @brief   ゲームオブジェクトイベントに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/07
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/EventSystem/EventData/EventData.h"

// クラスの前方宣言 ===================================================
class GameObject;
class ICollider;

// 列挙型の定義 ===============================================================
enum class GameObjectEventType 
{
	THROW_HIT, // 投げて衝突した時
	CAUGHT	,  // 捕まる
};

struct CollisionEventData 
	: public EventData
{
	GameObject* pHitGameObject;
	ICollider* pHitCollider;

	CollisionEventData(GameObject* pHitGameObject, ICollider* pHitCollider)
		: pHitGameObject{ pHitGameObject }
		, pHitCollider	{ pHitCollider }
	{
	}
};	

using ThrowHitEventData = CollisionEventData;

struct CaughtEventData
	: public EventData
{
	GameObject* captorObject;

	CaughtEventData(GameObject* captorObject)
		: captorObject {captorObject}
	{
	}
};