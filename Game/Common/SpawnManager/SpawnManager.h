/*****************************************************************//**
 * @file    SpawnManager.h
 * @brief   出現管理クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>
#include <functional>
#include "Game/Common/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

#include "Game/Common/TaskManager/TaskManager.h"

// クラスの前方宣言 ===================================================
class EnemyManager; // 敵管理
class Enemy;
class CommonResources; // 共通リソース
class CollisionManager;// 衝突管理
class EscapeHelicopter; // 脱出用ヘリコプター

// クラスの定義 ===============================================================
/**
 * @brief 出現管理
 */
class SpawnManager
	: public IGameFlowObserver
	, public Task
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	// オブジェクトプール (仮で持つ)
	std::vector < std::unique_ptr<Enemy>> m_enemyPool;	// 敵のオブジェクトプール

	// イベントスタック
	std::vector<std::function<void()>> m_eventStack;	///< イベントスタック

	EnemyManager* m_pEnemyManager;	///< 敵管理

	std::vector<std::unique_ptr<EscapeHelicopter>>* m_pEscapeHelicopters; ///< 脱出用ヘリコプター群

	// 簡易フラグ
	bool m_stoleTreasure;

	ElapsedTimeCounter	m_enemySpawnCounter;

	const CommonResources* m_pCommonResources;
	CollisionManager* m_pCollisionManager;

	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SpawnManager();

	// デストラクタ
	~SpawnManager();


// 操作
public:
	// 初期化処理
	void Initialize(EnemyManager* pEnemyManager, std::vector<std::unique_ptr<EscapeHelicopter>>* pEscapeHelicopters,const CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 終了処理
	void Finalize();

// イベント関連
public:
	

	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;


// 取得/設定
public:


// 内部実装
private:

	// 宝を盗まれた際の出現処理
	void OnStealingTreasures();

	void SpawnEnemy();
};
