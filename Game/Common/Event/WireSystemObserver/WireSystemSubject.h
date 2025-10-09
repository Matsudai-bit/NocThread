/*****************************************************************//**
 * @file    WireSystemSubject.h
 * @brief   ワイヤーシステムの観察対象に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
class IWireEventObserver;	// ワイヤーシステムの観察者
struct WireEventData;		// ワイヤのイベントデータ

namespace DirectX
{
	namespace SimpleMath
	{
		struct Vector3;
	}
}

// クラスの定義 ===============================================================
/**
 * @brief ワイヤーシステムの観察対象
 */
class WireSystemSubject
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::vector<IWireEventObserver*> m_observers;	///< 観察対象群

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	WireSystemSubject();

	// デストラクタ
	~WireSystemSubject();


// 通知
public:

	// ワイヤーに掴まれたときの通知
	void NotifyGrab		(const WireEventData& eventData);

	// ワイヤーから離れたときの通知
	void NotifyRelease	(const WireEventData& eventData);

	// ワイヤーで引き寄せられるときの通知
	void NotifyPull		(const WireEventData& eventData);

	// ワイヤーで投げられるときの通知
	void NotifyThrow	(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse);

	// ワイヤーに掴まれている時の通知
	void NotifyHover	(const WireEventData& eventData);


// 登録関連
public:

	// 観察者の登録
	void AddObserver(IWireEventObserver* pObserver);

	// 観察者の削除
	void RemoveObserver(IWireEventObserver* pObserver);

	// 観察者の登録
	void AllRemoveObserver();




// 取得/設定
public:


// 内部実装
private:


};
