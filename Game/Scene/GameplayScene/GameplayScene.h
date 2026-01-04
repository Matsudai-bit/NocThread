/**
 * @file   GameplayScene.h
 *
 * @brief  ゲームプレイシーンに関するヘッダファイル
 *
 * @author	松下大暉
 *
 * @date   2025/03/29
 */

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/Manager/SceneManager/SceneManager.h"

// ライブラリ
#include "Library/MyLib/DirectXMyToolKit/DebugCamera/DebugCamera.h"       // デバックカメラ
#include "Library/DirectXFramework/DebugDraw.h"         // デバック描画
#include "Library/MyLib/DirectXMyToolKit/GridFloor/GridFloor.h"         // デバック床

#include "Game/Common/Screen.h"

// システム
#include "Game/Common/CommonResources/CommonResources.h"                // 共通リソース
#include "Game/Common/EventSystem/EventData/EventData.h"
#include "Game/Common/EventSystem/EventSystem.h"
#include "Game/Common/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"
#include "Game/Common/StateMachine/StateMachine.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

// ゲームオブジェクト


// クラスの宣言 ===============================================================
class CommonResources;  // 共通リソース
class CollisionManager;	// 衝突判定管理
class GameEffectManager;// エフェクト管理
class StageManager;		// ステージ管理
class TaskManager;		// タスク管理
class Minimap;			// ミニマップ
class GameDirector;		// ゲーム進行の監督
class SpawnManager;		// 出現管理
class CollisionMatrix;	// 衝突検知する対応付け表


// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーン
 */
class GameplayScene 
	: public MyLib::Scene<CommonResources>
	, public IGameFlowObserver
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 紐のパラメータ
	static constexpr int	PARTICLE_NUM	= 50;
	static constexpr float	ROPE_LENGTH		= 1.5f;

	//static constexpr float	ROPE_FIXIBLILLITY = 0.00005f; 

// 正規版
	//static constexpr float	ROPE_FIXIBLILLITY = 0.000006f; 

	// 実験
	static constexpr float	ROPE_FIXIBLILLITY = 0.000000006f; 

// データメンバの宣言 -----------------------------------------------
private:

	// 状態
	std::unique_ptr<StateMachine<GameplayScene>> m_stateMachine; ///< ステートマシーン

    // システム
	std::unique_ptr<GameDirector>		m_gameDirector;		///< ゲーム進行の監督
    std::unique_ptr<CollisionManager>   m_collisionManager; ///< 衝突管理
	std::unique_ptr<GameEffectManager>	m_gameEffectManager;///< ゲームエフェクト管理
	std::unique_ptr<TaskManager>		m_taskManager;		///< タスク管理
	std::unique_ptr<SpawnManager>		m_spawnManager;		///< 出現管理
	std::unique_ptr<CollisionMatrix>	m_collisionMatrix;	///< 出現管理

	// その他
	std::unique_ptr<StageManager> m_stageManager;	///< ステージ上のオブジェクトを管理
	
	std::unique_ptr<Minimap>	m_miniMap; ///< ミニマップ

	std::vector <std::function<void()>> m_eventStack;
	ElapsedTimeCounter m_gamePlayingTimeCounter;		///< ゲームのプレイ時間カウンター


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GameplayScene();

	// デストラクタ
	~GameplayScene();


// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float deltaTime) override;

	// 描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;

	// イベントの解消
	void ResolveEvents();

	// ウインドウサイズに依存するリソースを作成する
	virtual void CreateWindowSizeDependentResources() override;
	void OnEndScene();

// 挙動関連
public:


// 通知関連
public:
	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;



// 取得/設定
public:

	// ステージ管理の取得
	StageManager* GetStageManager() const { return m_stageManager.get(); };

	// タスク管理の取得
	TaskManager* GetTaskManager() const { return m_taskManager.get();	}

// 内部実装
private:

	// --------------------------------------------------------------------
	// 初期設定関連 
	// --------------------------------------------------------------------
	// ゲーム開始時のセットアップ
	void SetUpForGameStart();

	// 基盤の作成
	void CreatePlatform();

	// 基盤のセットアップ
	void SetupPlatform();

	// ステージの生成
	void CreateStage();

	// タスクの作成
	void CreateTask();

	// ゲームを開始する
	void StartGame();


};

