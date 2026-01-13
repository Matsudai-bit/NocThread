/*****************************************************************//**
 * @file   EventSystem.h
 * @brief  イベントシステムに関するヘッダーファイル
 *
 * @author 松下大暉
 * @date   2025/07/06 
 *********************************************************************/

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <unordered_map> // イベントIDとリスナー、優先度マップのために使用
#include <vector>        // イベントキューとリスナーリストのために使用
#include <memory>        // イベントデータのユニークポインタのために使用
#include <typeindex>     // typeidのために使用
#include <functional>    // std::function のために必要
#include <algorithm>     // std::sort のために必要

// EventData.h 
#include "Game/Common/EventSystem/EventData/EventData.h"

// クラスの前方宣言 ===================================================
// 必要に応じてここに前方宣言を追加

// クラスの定義 ===============================================================
/**
 * @brief イベントシステム
 *
 * ゲーム内で発生するイベントの管理、発火、リスナーへの通知を行うシングルトン（またはインスタンス）クラス。
 * イベントはキューに格納され、ApplyEvents() が呼び出された際に処理される。
 * 複数の異なる優先度レベルをサポートする:
 * 1. イベントキュー内でのEventID自体の処理優先度
 * 2. 単一のEventIDに対するリスナーの呼び出し優先度
 *
 * @tparam TEventType イベントIDとして使用する型 (例: int, enum class)
 */
template <typename TEventType>
class EventSystem
{
    // エイリアス宣言 ---------------------------------------------------
private:
    /**
     * @brief イベントIDの型
     */
    using EventID = TEventType;
    /**
     * @brief イベントコールバック関数の型
     *
     * const EventData& を引数に取り、void を返す関数オブジェクト
     */
    using EventCallback = std::function<void(const EventData&)>;

    // 構造体の宣言 ---------------------------------------------------
private:
    /**
     * @brief リスナーエントリ構造体
     *
     * イベントIDに紐付けられる個々のリスナー（コールバック関数）とその呼び出し優先度を保持する。
     */
    struct ListenerEntry {
        EventCallback callback; ///< 呼び出されるコールバック関数
        int priority;           ///< このリスナー（コールバック）の呼び出し優先度 (数値が小さいほど優先度が高い)
    };

    /**
     * @brief キューに入れるイベントアイテム構造体
     *
     * 発火されたイベントのID、データ、およびそのイベント自体の処理優先度を保持する。
     * m_eventQueue に格納される。
     */
    struct QueuedEventItem {
        EventID id;                         ///< イベントのID
        std::unique_ptr<EventData> data;    ///< イベントに付随するデータ (unique_ptrで所有権を持つ)
        int eventPriority;                  ///< このイベント自体の処理優先度 (数値が小さいほど先に処理される)
    };


    // クラス定数の宣言 -------------------------------------------------
public:

// データメンバの宣言 -----------------------------------------------
private:
    std::vector<QueuedEventItem> m_eventQueue;                           ///< イベントのキュー（処理の依頼が来たイベントがたまる場所）
    std::unordered_map<EventID, std::vector<ListenerEntry>> m_listeners; ///< イベントIDごとのリスナーリスト (各IDに対し複数のリスナーとそれぞれの優先度)
    std::unordered_map<EventID, int> m_eventDefaultPriorities;           ///< イベントIDごとのデフォルト処理優先度 (EventSystemがEventIDから自動的に決定)
    bool m_isSortEventQueue;                                             ///< イベントキューがソートされているかを示すフラグ

    // メンバ関数の宣言 -------------------------------------------------
    // コンストラクタ/デストラクタ
public:
    /**
     * @brief コンストラクタ
     */
    EventSystem() : m_isSortEventQueue{ false } {}

    /**
     * @brief デストラクタ
     */
    ~EventSystem() = default;


   // 操作
public:
    /**
     * @brief イベントIDのデフォルト処理優先度を設定する
     *
     * FireEvent時にイベントキューへの追加されるイベントの処理順序を決定する優先度を設定する。
     * この優先度は、EventSystemがEventIDに基づいて自動的に決定するため、イベント発火側は意識しない。
     *
     * @param[in] id        優先度を設定するイベントID
     * @param[in] priority  設定する優先度 (数値が小さいほど先に処理される)
     */
    void SetEventDefaultPriority(EventID id, int priority) 
    {
        // 指定されたEventIDに対し、デフォルトの処理優先度をマップに登録する
        m_eventDefaultPriorities[id] = priority;
    }

    /**
     * @brief リスナーの登録
     *
     * 指定されたイベントIDに対し、コールバック関数をリスナーとして登録する。
     * リスナーは、そのイベントが発火した際に呼び出される。
     * 複数のリスナーが登録された場合、listenerPriorityに基づいて呼び出し順序が決定される。
     *
     * @tparam TEventData イベントデータとして期待される型 (EventDataの派生クラスである必要がある)
     * @tparam Func       コールバック関数の型 (std::function<void(const TEventData&)> と互換性がある型)
     * @param[in] id            登録するイベントのID
     * @param[in] callback      イベント発生時に呼び出されるコールバック関数
     * @param[in] listenerPriority このリスナー（コールバック）の呼び出し優先度 (数値が小さいほど先に呼び出される)
     */
    template<typename TEventData , typename Func>
    void AddListener(EventID id, Func callback, int listenerPriority = 0)
    {
        // TEventDataがEventDataの派生クラスであることをコンパイル時にチェック
        static_assert(std::is_base_of<EventData, TEventData>::value, "TEventData must be derived from EventData");

        // 指定されたEventIDに対応するリスナーリストに新しいリスナーを追加
        // コールバック関数をラップし、適切な型にキャストして呼び出すラムダを生成
        m_listeners[id].emplace_back(ListenerEntry{
            [callback, id](const EventData& data)
            {
                // 実行時に、渡されたイベントデータの型が登録時に期待された型と一致するかチェック
                if (typeid(data) == typeid(TEventData)) 
                {
                    // 型が一致すれば、安全にTEventData型にstatic_castしてコールバックを呼び出す
                    callback(static_cast<const TEventData&>(data));
                }
                else 
                {
                    // 型が一致しない場合のエラー出力 (開発中にのみ有効化推奨)
          /*          std::cerr << "EventSystem Error: Mismatched EventData type for ID " << id
                              << ". Expected: " << typeid(TEventData).name()
                              << ", Got: " << typeid(data).name() << std::endl;*/
                }
            },
            listenerPriority // このリスナーの呼び出し優先度を設定
            });

        // 同じEventIDに複数のリスナーが登録された場合、その呼び出し順序をlistenerPriorityに基づいてソートする
        // これにより、ApplyEvents時にリスナーを呼び出す際に再ソートする必要がなくなる
        auto it = m_listeners.find(id);
        if (it != m_listeners.end()) 
        {
            std::sort(it->second.begin(), it->second.end(),
                [](const ListenerEntry& a, const ListenerEntry& b) {
                    return a.priority < b.priority; // priorityが小さいリスナーほど先に呼び出される
                });
        }
    }
	// オーバーロード: 引数なしコールバック用
    void AddListener(EventID id, std::function<void()> callback, int listenerPriority = 0)
    {
		AddListener<EventData>(id,
			[callback](const EventData& data)
			{
				UNREFERENCED_PARAMETER(data);
				callback();
			},
			listenerPriority);
    }


    /**
 * @brief 特定のイベントIDに登録された全てのリスナーを削除する
 *
 * @param[in] id 削除するイベントのID
 */
    void RemoveListener(EventID id) // 関数名を変更せず、処理内容を「リスナーの削除」に修正
    {
        // m_listeners マップから、指定されたEventIDのエントリ（とそれに紐づくvectorのリスナー）を削除
        m_listeners.erase(id);
    }

    /**
     * @brief 全てのイベントIDに登録されたリスナーを全て削除する
     */
    void RemoveAllListeners() // 関数名を複数形に変更して、全てのリスナーを対象とすることを明確化
    {
        // m_listeners マップ全体をクリアする
        m_listeners.clear();
    }

    /**
     * @brief イベントキューから指定されたイベントIDのイベントを全て削除する（処理はしない）
     *
     * この関数は、リスナーの削除とは異なり、キューに積まれた発火済みのイベントを破棄します。
     * @param[in] id 削除するイベントのID
     */
    void RemoveQueuedEvents(EventID id) // 新しい関数名: キューからイベントを削除する目的を明確に
    {
        // イベントキューから指定IDのイベントを全て削除
        m_eventQueue.erase(std::remove_if(m_eventQueue.begin(), m_eventQueue.end(),
            [&id](const QueuedEventItem& queuedEventItem) {
                return queuedEventItem.id == id; // 指定IDのイベントを削除対象とする
            }),
            m_eventQueue.end());

        // キューが変更されたため、ソート状態を「未ソート」に設定
        m_isSortEventQueue = false;
    }

    /**
     * @brief イベントキューからイベントを全て削除する（処理はしない）
     */
    void RemoveAllQueueEvents()
    {
        m_eventQueue.clear();
        // キューが変更されたため、ソート状態を「未ソート」に設定
        m_isSortEventQueue = false;
    }
    /**
     * @brief イベントの発火
     *
     * 指定されたイベントIDとイベントデータをイベントキューに追加する。
     * イベントの処理優先度は、EventSystemに事前に設定されたEventIDごとのデフォルト優先度に基づいて自動的に決定される。
     *
     * @param[in] id    発火するイベントのID
     * @param[in] data  イベントに付随するデータ (unique_ptrで所有権を渡す)
     */
    void FireEvent(EventID id, std::unique_ptr<EventData> data = std::make_unique<EventData>())
    {
        // イベントデータが有効であることを確認
        if (data) 
        {
            // イベントIDに対応するデフォルトの処理優先度を取得
            int eventPriority = 0; // 優先度が設定されていない場合のデフォルト値
            auto it = m_eventDefaultPriorities.find(id);
            if (it != m_eventDefaultPriorities.end()) 
            {
                eventPriority = it->second; // 登録されている優先度を使用
            }
            else 
            {
                // 警告: 未登録のEventIDに対する優先度設定がない場合
                // デバッグモードでのみ出力推奨
                //std::cerr << "EventSystem Warning: No default priority set for EventID " << id
                //    << ". Using default priority 0." << std::endl;
            }

            // イベントをキューに追加 (ID, データ, 処理優先度)
            m_eventQueue.push_back({ id, std::move(data), eventPriority });

            // イベントキューに新しいイベントが追加されたため、ソート状態を「未ソート」に設定
            m_isSortEventQueue = false;
        }
    }
 

    /**
     * @brief 全てのイベントの適用
     *
     * イベントキューに積まれた全てのイベントを、それぞれのEventIDの処理優先度に基づいてソートし、処理する。
     * 各イベントに対し、登録されているリスナーをリスナーの呼び出し優先度に基づいて呼び出す。
     * 処理後、イベントキューはクリアされる。
     */
    void ApplyEvents()
    {
        // イベントキューが未ソート状態であれば、EventIDの処理優先度に基づいてソートする
        if (m_isSortEventQueue == false) 
        {
            CompareByEventQueueAscending();
        }

        // ソートされたイベントキューを一つずつ処理
        for (auto& queuedEventItem : m_eventQueue) 
        {
            EventID currentID = queuedEventItem.id;
            EventData& currentData = *(queuedEventItem.data); // unique_ptrから生参照を取得

            // 現在のイベントIDにリスナーが登録されているかチェック
            auto it = m_listeners.find(currentID);
            if (it != m_listeners.end()) 
            {
                // 登録されているリスナーリストを、リスナーの呼び出し優先度に基づいて順に呼び出す
                // リスナーリストはAddListener時に既にソートされている
                for (const auto& listenerEntry : it->second) 
                {
                    listenerEntry.callback(currentData);
                }
            }
        }
        m_eventQueue.clear(); // 全てのイベント処理後、キューをクリア
    }

    /**
     * @brief 指定したイベントの適用
     *
     * イベントキューから指定されたEventIDのイベントのみを検索し、処理する。
     * 処理後、該当するイベントはキューから削除される。
     * 他のイベントはキューに残る。
     *
     * @param[in] id 処理するイベントのID
     */
    void ApplyEvent(EventID id)
    {
        // イベントキューが未ソート状態であれば、EventIDの処理優先度に基づいてソートする
        if (m_isSortEventQueue == false) 
        {
            CompareByEventQueueAscending();
        }

        // イベントキューをループし、指定されたIDのイベントを探し処理する
        for (auto& queuedEventItem : m_eventQueue)
        {
            // 指定されたIDと異なるイベントはスキップ
            if (queuedEventItem.id != id) continue;

            EventID currentID = queuedEventItem.id;
            EventData& currentData = *(queuedEventItem.data);

            // リスナーが存在するかチェック
            auto it = m_listeners.find(currentID);
            if (it != m_listeners.end())
            {
                // 登録されているリスナーを優先度順に呼び出す
                for (const auto& listenerEntry : it->second)
                {
                    listenerEntry.callback(currentData);
                }
            }
        }

        // 処理した指定IDのイベントをキューからまとめて削除
        // これにより、削除後にキューのソート状態が失われる可能性があるため、フラグをリセットする
        m_eventQueue.erase(std::remove_if(m_eventQueue.begin(), m_eventQueue.end(),
            [&id](const QueuedEventItem& queuedEventItem) {
                return queuedEventItem.id == id; // 指定IDのイベントを削除対象とする
            }),
            m_eventQueue.end());

        // キューが変更されたため、ソート状態を「未ソート」に設定
        m_isSortEventQueue = false;
    }

    // 内部実装 -------------------------------------------------
private:
    /**
     * @brief イベントキューをEventIDの処理優先度に基づいて昇順にソートする
     *
     * m_eventQueue に新しいイベントが追加された場合など、キューの処理順序を保証するために呼び出される。
     */
    void CompareByEventQueueAscending()
    {
        // m_eventQueue の各要素 (QueuedEventItem) の eventPriority を比較してソート
        std::sort(m_eventQueue.begin(), m_eventQueue.end(),
            [](const QueuedEventItem& a, const QueuedEventItem& b) {
                return a.eventPriority < b.eventPriority; // eventPriorityが小さいほど先に処理される
            });

        // ソートが完了したことを示すフラグを設定
        m_isSortEventQueue = true;
    }
};

