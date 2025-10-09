/*****************************************************************//**
 * @file    EventData.h
 * @brief   イベントデータに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/06
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================

enum class GameObjectEventCategory
{
	Collision = 1,
	Damage,
	Death,
};

//enum class EventCategory
//{
//	Collision = 1,
//	Damage,
//	Death, 
//};


/**
 * @brief イベントIDを生成するヘルパー関数
 * 
 * @param[in] category イベントのカテゴリ
 * @param[in] subType　イベントの名前
 * 
 * @return イベントID
 */
template <typename TEventCategory>
constexpr int GenerateEventID(TEventCategory category, int subType) {
	// 適切な倍率でカテゴリとサブタイプを結合
	// 例: Collision(1) * 1000 + Enter(1) -> 1001
	//     Damage(2)   * 1000 + Physical(1) -> 2001
	// あるいは、カテゴリごとに異なるオフセットを使うなど、より複雑なロジックも可能
	return static_cast<int>(category) * 1000 + subType; // 100の代わりに1000を使うと、各カテゴリで999種類のイベントを定義できる
}

// クラスの定義 ===============================================================
/**
 * @brief イベントデータ
 */
struct EventData
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
public:


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EventData();

	// デストラクタ
	virtual ~EventData();
};

enum class TestEventTyoe
{
	Collision = 1,
	Damage,
	Death,
};

class GameObject;
struct TestEventData 
	: public EventData
{
	GameObject* sender;
	GameObject* other;

	TestEventData(GameObject* sender, GameObject*  other)
		: sender{sender}
		, other{other}
	{ }


};



class TestEvent
{

};
