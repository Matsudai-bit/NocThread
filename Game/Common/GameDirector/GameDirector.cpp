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

#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/TransitionMask/TransitionMask.h"
#include "Game/Common/ResultData/ResultData.h"

#include "Game/Scene/ResultScene/ResultScene.h"
#include "Game/Scene/Loading/LoadingScreen.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameDirector::GameDirector()
	: m_pCommonResources	{ nullptr }
	, m_isFadeOutInProgress	{ false }
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
void GameDirector::Initialize(const CommonResources* pCommonResources)
{
	m_pCommonResources = pCommonResources;

	m_gameProgressDataManager->ResetData();
	SetUpEventFlowMap();
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
	UNREFERENCED_PARAMETER(deltaTime);

	// フェードアウト中か確認する
	if (m_isFadeOutInProgress && m_pCommonResources->GetTransitionMask()->IsEnd())
	{
		// フェードアウト完了後の処理
		GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_TRANSITION_FADING_FINISH);
		
	}
	
	// ゲームフローイベントの解消
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
	auto resolveStack = m_gameFlowEventStack;

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
		case GameFlowEventID::GAME_TRANSITION_FADING_START:
			// フェードアウト開始
			m_isFadeOutInProgress = true;
			break;
		case GameFlowEventID::GAME_TRANSITION_FADING_FINISH:
			// フェードアウト完了
			m_isFadeOutInProgress = false;
			break;
		case GameFlowEventID::GAME_END:
		default:
			break;
		}

	}

	// 次のイベントが予約されているか確認する　
	while (!resolveStack.empty())
	{		
		// 要素の取得
		auto eventID = resolveStack.top();
		// 要素の削除
		resolveStack.pop();
		if (m_eventFlowMap.find(eventID) != m_eventFlowMap.end())
		{
			// 次のイベントをスタックに追加する
			GameFlowMessenger::GetInstance()->Notify(m_eventFlowMap[eventID]);
		}
	}


}

/**
 * @brief イベントの流れを管理するマップの設定
 * 
 */
void GameDirector::SetUpEventFlowMap()
{
	// イベントの流れを設定する

	// **** ゲーム開始 ****
	m_eventFlowMap[GameFlowEventID::GAME_SETUP_FINISH] = GameFlowEventID::GAME_START;
	m_eventFlowMap[GameFlowEventID::GAME_START] = GameFlowEventID::GAME_TRANSITION_FADING_START;

	// **** ゲーム終了 ****
	m_eventFlowMap[GameFlowEventID::ESCAPE_SUCCESS] = GameFlowEventID::GAME_TRANSITION_FADING_START;

	m_eventFlowMap[GameFlowEventID::PLAYER_DIE] = GameFlowEventID::GAME_TRANSITION_FADING_START;
	
}

