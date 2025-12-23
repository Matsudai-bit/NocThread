/*****************************************************************//**
 * @file    GameDirector.h
 * @brief   ゲーム進行を管理する監督に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameDirector.h"

#include "Game/Common/GameProgressData/GameProgressData.h"

#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"	// ゲームフローの監視者ようインターフェース


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameDirector::GameDirector()
{
	// 自身の登録
	GameFlowMessenger::GetInstance()->RegistryObserver(this);

	// ゲーム進捗データ管理
	m_gameProgressDataManager = std::make_unique<GameProgressDataManager>();
}



/**
 * @brief デストラクタ
 */
GameDirector::~GameDirector()
{

}

/**
 * @brief 初期化処理
 */
void GameDirector::Initialize()
{
	m_gameProgressDataManager->ResetData();
}

/**
 * @brief タスクの更新処理
 * 
 * @param[in] deltaTime　前フレームからの経過時間
 * 
 * @returns true  タスクの継続
 * @returns false タスクの破棄
 */
bool GameDirector::UpdateTask(float deltaTime)
{
	ResolveGameFlowEvent();

	return true;
}


/**
 * @brief イベントメッセージを受け取る
 * 
 * @param[in] eventID イベントID
 */
void GameDirector::OnGameFlowEvent(GameFlowEventID eventID)
{
	// イベントスタックに追加する
	m_gameFlowEventStack.push(eventID);
	
}

/**
 * @brief ゲームフローイベントの解消
 * 
 */
void GameDirector::ResolveGameFlowEvent()
{
	// イベントを一気に解消する
	while (!m_gameFlowEventStack.empty())
	{
		// 要素の取得
		auto eventID = m_gameFlowEventStack.top();
		// 要素の削除
		m_gameFlowEventStack.pop();

		switch (eventID)
		{
		case GameFlowEventID::GAME_START:
			break;
		case GameFlowEventID::STOLE_TREASURE:
			break;
		case GameFlowEventID::CHECKPOINT_PASSED:
			m_gameProgressDataManager->IncrementPassedCheckpoints();
			if (m_gameProgressDataManager->GetProgressData().passedCheckpointNum > 0)
			{
				GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::STOLE_TREASURE);
			}
			break;
		case GameFlowEventID::SPAWN_HELICOPTER:
			break;
		case GameFlowEventID::PLAYER_DIE:
			break;
		case GameFlowEventID::ESCAPE_SUCCESS:
			break;
		default:
			break;
		}
	}
}

