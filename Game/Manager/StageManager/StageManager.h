/*****************************************************************//**
 * @file    StageManager.h
 * @brief   ステージ管理に関するヘッダーファイル
 *			インゲームのステージの生成とオブジェクト管理を請け負う
 *
 * @author  松下大暉
 * @date    2025/11/117
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <nlohmann/json.hpp>

// ライブラリ
#include "Library/MyLib/DirectXMyToolKit/DebugCamera/DebugCamera.h"       // デバックカメラ
#include "Library/DirectXFramework/DebugDraw.h"         // デバック描画
#include "Library/MyLib/DirectXMyToolKit/GridFloor/GridFloor.h"         // デバック床

#include "Game/Common/TaskManager/TaskManager.h"// タスク（基底クラス）
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
#include "Game/GameObjects/RopeObject/XPBDSimulator/XPBDSimulator.h"
#include "Game/GameObjects/RopeObject/ParticleObject/ParticleObject.h"

// クラスの宣言 ===============================================================
class CommonResources;  // 共通リソース
class Floor;			// 床
class CollisionManager;	// 衝突判定管理
class SpawnManager;		// 出現管理
class EnemyManager;		// 敵管理
class Wall;				// 壁
class Segment;			// 線分
class Building;			// 建物
class PlayerCamera;		// プレイヤーのカメラ
class Canvas;			// キャンバス
class Sprite;			// スプライト
class StageObject;		// ステージオブジェクト
class Enemy;			// 敵
class Treasure;			// 宝
class EscapeHelicopter;	// ヘリコプター
class IEnemyFactory;
class CircularShadow;	// 丸影
class BuildingManager;  // 建物管理
class PlayerManager;	// プレイヤー管理

class RopeObject;
class ParticleObject;
class XPBDSimulator;
class GameEffectManager;
class TaskManager;		// タスク管理


// クラスの定義 ===============================================================
/**
 * @brief ステージ管理
 */
class StageManager 
	: public Task
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 紐のパラメータ
	static constexpr int	PARTICLE_NUM = 50;
	static constexpr float	ROPE_LENGTH = 1.5f;

	//static constexpr float	ROPE_FIXIBLILLITY = 0.00005f; 

// 正規版
	//static constexpr float	ROPE_FIXIBLILLITY = 0.000006f; 

	// 実験
	static constexpr float	ROPE_FIXIBLILLITY = 0.000000006f;


	// --- カメラ設定関連 ---
	static constexpr float CAMERA_FOV_DEGREES = 45.0f;	///< 射影行列の視野角 (度)
	static constexpr float CAMERA_NEAR_CLIP = 0.1f;		///< 射影行列のニアクリップ距離
	static constexpr float CAMERA_FAR_CLIP = 450.0f;	///< 射影行列のファークリップ距離

	// --- UI関連 (スコープ) ---
	static constexpr const char*	SCOPE_TEXTURE_PATH = "scope.dds"; ///< スコープスプライトのテクスチャファイルパス
	static constexpr float			SCOPE_Y_OFFSET = 140.0f;		///< スコープスプライトのY軸調整オフセット
	static constexpr float			SCOPE_SCALE = 0.09f;		///< スコープスプライトのスケール


	// --- スカイボックス関連 ---
	static constexpr float SKYSPHERE_SCALE = 300.0f;	///< 天球（スカイスフィア）のスケール値

	// --- ステージ配置関連 (お宝候補地) ---
	// お宝候補地1
	static constexpr DirectX::SimpleMath::Vector3 TREASURE_POS_CANDIDATE_1 = { 20.0f, 42.0f, -70.0f };
	// お宝候補地2
	static constexpr DirectX::SimpleMath::Vector3 TREASURE_POS_CANDIDATE_2 = { -120.0f, 55.6f, -240.2f };
	// お宝候補地3
	static constexpr DirectX::SimpleMath::Vector3 TREASURE_POS_CANDIDATE_3 = { 190.0f, 50.6f, -70.2f };

	// ステージデータフォルダ
	const std::string STAGE_DATA_FOLDER_PATH = "Resources/Data/MainStage";

	// データメンバの宣言 -----------------------------------------------
private:


	// リソース関連
	const CommonResources* m_pCommonResources;


	// グリッドの床
	std::unique_ptr<MyLib::GridFloor> m_gridFloor;

	std::unique_ptr<PlayerCamera>		m_playerCamera;

	// スプライト関連

	// ゲームオブジェクト管理系
	std::unique_ptr<SpawnManager>	m_spawnManager;		///< 生成管理
	std::unique_ptr<EnemyManager>	m_enemyManager;		///< 敵管理
	std::unique_ptr<BuildingManager>m_buildingManager;	///< 建物管理
	std::unique_ptr<PlayerManager>	m_playerManager;	///< プレイヤー管理


	// ゲームオブジェクト
	std::unique_ptr<Floor>						m_floor;		///< 床
	std::vector<std::unique_ptr<StageObject>>	m_stageObject;	///< ステージオブジェクト
	std::unique_ptr<Treasure>					m_treasure;		///< お宝
	std::vector<std::unique_ptr<Building>>		m_buildings;	////< 建物
	std::vector< std::unique_ptr<EscapeHelicopter>>				m_escapeHelicopter;///< 脱出用ヘリコプター

	// その他
	DirectX::Model m_skySphere;	///< 天球

	std::vector <std::function<void()>> m_eventStack;

	bool m_isStoppingUpdate; ///< 更新処理を止めるかどうか


public:
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

	// ここまで　=================================================-

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageManager(const CommonResources* pCommonResources);

	// デストラクタ
	~StageManager();


	// 初期化処理
	void Initialize() ;

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

	// 終了処理
	void Finalize() ;

	// ウインドウサイズに依存するリソースを作成する
	void CreateWindowSizeDependentResources() ;

	// ステージ作成
	void CreateStage(CollisionManager* pCollisionManager, TaskManager* pTaskManager);

	// タスクの追加
	void AddTask(TaskManager* pTaskManager);

	// 更新処理を止める
	void StopUpdating();

	// 更新処理を開始する
	void StartUpdating();

	// 挙動関連
private:
	// インゲームのゲームオブジェクトを更新する
	void UpdateInGameObjects(float deltaTime);
	// インゲームのゲームオブジェクトを描画する
	void DrawInGameObjects(const Camera& camera);

	// 通知関連
public:



	// 取得/設定
public:


	// 内部実装
private:

	void OnEndScene();

	// プレイヤーの作成
	void CreatePlayer(PlayerData data, CollisionManager* pCollisionManager);


};
