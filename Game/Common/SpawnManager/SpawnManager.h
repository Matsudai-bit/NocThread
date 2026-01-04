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
class EnemyManager;		// 敵管理
class PlayerManager;	// プレイヤー管理
class BuildingManager;	// 建物管理

class Enemy;
class CommonResources; // 共通リソース
class CollisionManager;// 衝突管理
class EscapeHelicopter; // 脱出用ヘリコプター
class PlayerCamera;

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

	const std::string STAGE_DATA_FOLDER_PATH = "Resources/Data/MainStage";


// データメンバの宣言 -----------------------------------------------
private:

	// ゲームオブジェクトの参照管理
	EnemyManager*		m_pEnemyManager;	///< 敵管理
	PlayerManager*		m_pPlayerManager;	///< プレイヤー管理
	BuildingManager*	m_pBuildingManager;	///< 建物管理
	PlayerCamera*		m_pPlayerCamera;	///< プレイヤーカメラ
	std::vector<std::unique_ptr<EscapeHelicopter>>* m_pEscapeHelicopters; ///< 脱出用ヘリコプター群

	// オブジェクトプール (仮で持つ)
	std::vector < std::unique_ptr<Enemy>> m_enemyPool;	// 敵のオブジェクトプール

	// イベントスタック
	std::vector<std::function<void()>> m_eventStack;	///< イベントスタック



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

	// 仮 ==========================================-
	struct PlayerData
	{
		int tileNumber;
	};
	struct StageLayoutData
	{
		std::string buildingJsonName;
		std::string playerJsonName;
	};

// 操作
public:
	// 初期化処理
	void Initialize(
		const CommonResources* pCommonResources, 
		CollisionManager* pCollisionManager);

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 終了処理
	void Finalize();

	// ゲームの初期配置をする
	void SetupInitialLayout();

	// ゲームオブジェクト管理の設定
	void SetManagers(
		PlayerManager* pPlayerManager,
		BuildingManager* pBuildingManager,
		EnemyManager* pEnemyManager,
		std::vector<std::unique_ptr<EscapeHelicopter>>* pEscapeHelicopters,
		PlayerCamera* pPlayerCamera);

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
