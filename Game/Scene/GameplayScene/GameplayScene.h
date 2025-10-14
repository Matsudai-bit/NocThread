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
#include "Library/ImaseLib/DebugCamera.h"       // デバックカメラ
#include "Library/ImaseLib/DebugDraw.h"         // デバック描画
#include "Library/ImaseLib/GridFloor.h"         // デバック床

#include "Game/Common/Screen.h"


// システム
#include "Game/Common/CommonResources/CommonResources.h"                // 共通リソース
#include "Game/Common/EventSystem/EventData/EventData.h"
#include "Game/Common/EventSystem/EventSystem.h"
#include "Game/Common/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"
#include "Game/Common/StateMachine/StateMachine.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

// ゲームオブジェクト
#include "Game/GameObjects/RopeObject/RopeObject.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/XPDBSimulator.h"
#include "Game/GameObjects/RopeObject/ParticleObject/ParticleObject.h"

// クラスの宣言 ===============================================================
class CommonResources;  // 共通リソース
class Floor;			// 床
class CollisionManager;	// 衝突判定管理
class SpawnManager;		// 出現管理
class EnemyManager;		// 敵管理
class Wall;				// 壁
class Segment;			// 線分
class Player;			// プレイヤー
class Building;			// 建物
class PlayerCamera;		// プレイヤーのカメラ
class PlayerController;	// プレイヤーコントローラ
class Canvas;			// キャンバス
class Sprite;			// スプライト
class StageObject;		// ステージオブジェクト
class Enemy;			// 敵
class Treasure;			// 宝
class EscapeHelicopter;	// ヘリコプター
class IEnemyFactory;	
class CircularShadow;	// 丸影

class RopeObject;
class ParticleObject;
class XPBDSimulator;
class GameEffectManager;


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

	// リソース関連
	

    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // グリッドの床
    std::unique_ptr<Imase::GridFloor> m_gridFloor;
    // デバッグカメラ
    std::unique_ptr<Imase::DebugCamera> m_debugCamera;
    std::unique_ptr<PlayerCamera>		m_playerCamera;

    // システム
    std::unique_ptr<CollisionManager>   m_collisionManager; ///< 衝突管理
	std::unique_ptr<PlayerController>	m_playerController;	///< プレイヤーのコントローラ
	std::unique_ptr<GameEffectManager>	m_gameEffectManager;///< ゲームエフェクト管理

	// スプライト関連
	std::unique_ptr<Canvas> m_canvas; // スプライト表示用
	std::unique_ptr<Sprite> m_scopeSprite; // スプライト

	// ゲームオブジェクト管理系
	std::unique_ptr<SpawnManager>	m_spawnManager;	///< 生成管理
	std::unique_ptr<EnemyManager>	m_enemyManager;	///< 敵管理


	// ゲームオブジェクト
	std::unique_ptr<Floor>						m_floor;		///< 床
	std::vector<std::unique_ptr<Wall>>			m_walls;		///< 壁
	std::unique_ptr<Player>						m_player;		///< プレイヤー
	std::vector<std::unique_ptr<StageObject>>	m_stageObject;	///< ステージオブジェクト
	std::unique_ptr<Treasure>					m_treasure;		///< お宝
	std::vector<std::unique_ptr<Building>>		m_buildings;	////< 建物
	std::vector< std::unique_ptr<EscapeHelicopter>>				m_escapeHelicopter;///< 脱出用ヘリコプター

	// その他
	DirectX::Model m_skySphere;	///< 天球
	std::unique_ptr<CircularShadow> m_playerShadow;	///< プレイヤーの影

	std::vector <std::function<void()>> m_eventStack;
	ElapsedTimeCounter m_gamePlayingTimeCounter; ///< ゲームのプレイ時間の

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
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;

	// ウインドウサイズに依存するリソースを作成する
	virtual void CreateWindowSizeDependentResources() override;

// 挙動関連
public:
	// インゲームのゲームオブジェクトを更新する
	void UpdateInGameObjects(float elapsedTime);
	// インゲームのゲームオブジェクトを描画する
	void DrawInGameObjects();

// 通知関連
public:
	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;



// 取得/設定
public:


// 内部実装
private:

	void OnEndScene();

	void CreateStage();

};
