/*****************************************************************//**
 * @file    GameDirector.h
 * @brief   ゲーム進行を管理する監督に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <stack>
#include "Game/Common/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"	// ゲームフローの監視者ようインターフェース
#include "Game/Common/TaskManager/TaskManager.h"								// タスク管理

// クラスの前方宣言 ===================================================
class GameProgressDataManager; // ゲーム進捗データ管理
class CommonResources;		// 共通リソース

// クラスの定義 ===============================================================
/**
 * @brief ゲーム進行を管理する監督
 */
class GameDirector
	: public IGameFlowObserver
	, public Task
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	std::unique_ptr<GameProgressDataManager> m_gameProgressDataManager; ///< ゲーム進捗データ管理

	std::stack<GameFlowEventID> m_gameFlowEventStack;					///< ゲームフローに関するイベントスタック

	std::unordered_map<GameFlowEventID, GameFlowEventID> m_eventFlowMap;	///< イベントの流れを管理するマップ (キー: イベントID、値: 次のイベントID)

	const CommonResources* m_pCommonResources;			///< 共通リソース
	bool m_isFadeOutInProgress;			///< フェードが進行中かどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GameDirector();

	// デストラクタ
	~GameDirector();


// 操作
public:
	
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources);

	// タスクの更新処理
	bool UpdateTask(float deltaTime);

	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;


// 取得/設定
public:


// 内部実装
private:
	// ゲームフローイベントの解消
	void ResolveGameFlowEvent();

	// イベントフローマップの設定
	void SetUpEventFlowMap();
};
