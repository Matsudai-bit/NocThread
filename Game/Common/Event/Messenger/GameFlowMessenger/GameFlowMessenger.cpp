/*****************************************************************//**
 * @file    GameFlowMessenger.h
 * @brief   ゲーム進行のイベントメッセージに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameFlowMessenger.h"

#include "Game/Common/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"

// 静的変数の初期化
std::unique_ptr<GameFlowMessenger> GameFlowMessenger::s_instance = nullptr;

// メンバ関数の定義 ===========================================================


/**
 * @brief 監視者の登録
 * 
 * @param[in] observer　登録する監視者
 */
void GameFlowMessenger::RegistrObserver(IGameFlowObserver* observer)
{
    // 登録されていなければ登録
    m_observers.push_back(observer);
    
}

/**
 * @brief 通知
 * 
 * @param[in] eventID　イベントID
 */
void GameFlowMessenger::Notify(GameFlowEventID eventID)
{
    for (auto observer : m_observers)
    {
        observer->OnGameFlowEvent(eventID);
    }
}

/**
 * @brief 全て削除
 * 
 */
void GameFlowMessenger::RemoveAllObserver()
{
    m_observers.clear();
}

/**
 * @brief インスタンスの取得
 * 
 * @return インスタンス
 */
GameFlowMessenger* GameFlowMessenger::GetInstance()
{
    if (s_instance.get() == nullptr)
    {
        s_instance.reset(new GameFlowMessenger());
    }

    return s_instance.get();
}
