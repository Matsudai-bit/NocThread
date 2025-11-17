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

class StageManager;		// ステージ管理

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


	// --- カメラ設定関連 ---
	static constexpr float CAMERA_FOV_DEGREES	= 45.0f;	///< 射影行列の視野角 (度)
	static constexpr float CAMERA_NEAR_CLIP		= 0.1f;		///< 射影行列のニアクリップ距離
	static constexpr float CAMERA_FAR_CLIP		= 450.0f;	///< 射影行列のファークリップ距離

	// --- UI関連 (スコープ) ---
	static constexpr const char* SCOPE_TEXTURE_PATH = "scope.dds"; ///< スコープスプライトのテクスチャファイルパス
	static constexpr float SCOPE_Y_OFFSET			= 140.0f;		///< スコープスプライトのY軸調整オフセット
	static constexpr float SCOPE_SCALE				= 0.09f;		///< スコープスプライトのスケール





// データメンバの宣言 -----------------------------------------------
private:

	// 状態
	std::unique_ptr<StateMachine<GameplayScene>> m_stateMachine; ///< ステートマシーン

    // 射影行列
    DirectX::SimpleMath::Matrix m_projection;

    // グリッドの床
    std::unique_ptr<Imase::GridFloor> m_gridFloor;
    // デバッグカメラ
    std::unique_ptr<Imase::DebugCamera> m_debugCamera;

    // システム
    std::unique_ptr<CollisionManager>   m_collisionManager; ///< 衝突管理
	std::unique_ptr<GameEffectManager>	m_gameEffectManager;///< ゲームエフェクト管理
	

	// スプライト関連
	std::unique_ptr<Canvas> m_canvas; // スプライト表示用
	std::unique_ptr<Sprite> m_scopeSprite; // スプライト

	// その他
	std::unique_ptr<StageManager> m_stageManager;

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

	// ウインドウサイズに依存するリソースを作成する
	virtual void CreateWindowSizeDependentResources() override;

// 挙動関連
public:
	// インゲームのゲームオブジェクトを更新する
	void UpdateInGameObjects(float deltaTime);
	// インゲームのゲームオブジェクトを描画する
	void DrawInGameObjects();

// 通知関連
public:
	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;



// 取得/設定
public:

	// キャンバスの取得
	Canvas* GetCanvas() const;

	// ステージ管理の取得
	StageManager* GetStageManager() const;

// 内部実装
private:

	void OnEndScene();

	void CreateStage();

};
