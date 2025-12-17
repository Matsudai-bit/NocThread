/*****************************************************************//**
 * @file    CheckpointManager.h
 * @brief   チェックポイント管理に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/TaskManager/TaskManager.h"

// クラスの前方宣言 ===================================================
class Checkpoint;		// チェックポイントクラス
class CommonResources;	// 共通リソース
class CollisionManager; // 衝突管理

// クラスの定義 ===============================================================
/**
 * @brief チェックポイント管理
 */
class CheckpointManager
	: public Task
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	std::vector<std::unique_ptr<Checkpoint>> m_checkpoints; ///< チェックポイント


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CheckpointManager();

	// デストラクタ
	~CheckpointManager();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources);

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

	// 終了処理
	void Finalize();

	void CreateCheckpoint(const DirectX::SimpleMath::Vector3& position,CollisionManager* pCollisionManager, const CommonResources* pCommonResources);

// 取得/設定
public:


// 内部実装
private:


};
