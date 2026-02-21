/*****************************************************************//**
 * @file    WireSystemSubject.h
 * @brief   ワイヤーシステムの観察対象に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "WireSystemSubject.h"


// フレームワーク関連
#include "Game/Common/Framework/Event/WireSystemObserver/IWireEventObserver.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WireSystemSubject::WireSystemSubject()
{

}



/**
 * @brief デストラクタ
 */
WireSystemSubject::~WireSystemSubject()
{
	m_observers.clear();
}

/**
 * @brief 掴む通知
 * 
 * @param[in] eventData
 */
void WireSystemSubject::NotifyGrab(const WireEventData& eventData)
{

	for (IWireEventObserver* observer : m_observers)
	{
		observer->OnWireGrab(eventData);
	}
}
/**
 * @brief 離す通知
 *
 * @param[in] eventData　イベントデータ
 */
void WireSystemSubject::NotifyRelease(const WireEventData& eventData)
{
	for (IWireEventObserver* observer : m_observers)
	{
		observer->OnWireRelease(eventData);
	}
}

/**
 * @brief 引く通知
 *
 * @param[in] eventData　イベントデータ
 */
void WireSystemSubject::NotifyPull(const WireEventData& eventData)
{
	for (IWireEventObserver* observer : m_observers)
	{
		observer->OnWirePull(eventData);
	}
}

/**
 * @brief 投げる通知
 *
 * @param[in] eventData　イベントデータ
 */
void WireSystemSubject::NotifyThrow(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse)
{
	for (IWireEventObserver* observer : m_observers)
	{
		observer->OnWireThrow(eventData, throwImpulse);
	}
}

/**
 * @brief 掴んでいる状態
 *
 * @param[in] eventData
 */
void WireSystemSubject::NotifyHover(const WireEventData& eventData)
{
	for (IWireEventObserver* observer : m_observers)
	{
		observer->OnWireHover(eventData);
	}
}

/**
 * @brief 観察者の登録
 * 
 * @param[in] pObserver　オブザーバー
 */
void WireSystemSubject::AddObserver(IWireEventObserver* pObserver)
{
	// すでに登録されているかどうかの確認
	auto it = std::find(m_observers.begin(), m_observers.end(), pObserver);
	if (it != m_observers.end()) return;

	// 登録
	m_observers.push_back(pObserver);
}

/**
 * @brief 観察者の登録
 * 
 * @param[in] pObserver　オブザーバー
 */
void WireSystemSubject::RemoveObserver(IWireEventObserver* pObserver)
{
	// 登録されているかどうかの確認
	auto it = std::find(m_observers.begin(), m_observers.end(), pObserver);
	if (it == m_observers.end()) return;

	// 削除
	m_observers.erase(it);
}

/**
 * @brief すべての観察者の削除
 * 
 */
void WireSystemSubject::AllRemoveObserver()
{
	m_observers.clear();
}

