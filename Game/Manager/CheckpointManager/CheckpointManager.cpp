/*****************************************************************//**
 * @file    CheckpointManager.h
 * @brief   チェックポイント管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CheckpointManager.h"

#include "Game/GameObjects/CheckPoint/Checkpoint.h"
#include "Game/Common/CommonResources/CommonResources.h"
// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CheckpointManager::CheckpointManager()
{

}



/**
 * @brief デストラクタ
 */
CheckpointManager::~CheckpointManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CheckpointManager::Initialize(const CommonResources* pCommonResources)
{

}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　前フレームからの経過時間
 * 
 * @returns true	タスクの継続
 * @returns false	タスクの破棄
 */
bool CheckpointManager::UpdateTask(float deltaTime)
{
	for (auto& checkpoint : m_checkpoints)
	{		
		// 初期化処理
		checkpoint->Update(deltaTime);	
	}

	return true;
}

/**
 * @brief タスクの描画処理
 * 
 * @param[in] camera　カメラ
 */
void CheckpointManager::DrawTask(const Camera& camera)
{
	for (auto& checkpoint : m_checkpoints)
	{		
		// 初期化処理
		checkpoint->Draw(camera);
		
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CheckpointManager::Finalize()
{

}

void CheckpointManager::CreateCheckpoint(const DirectX::SimpleMath::Vector3& position, CollisionManager* pCollisionManager,const CommonResources* pCommonResources)
{
	m_checkpoints.emplace_back(std::move(std::make_unique<Checkpoint>()));

		// 初期化処理
	m_checkpoints.back()->Initialize(pCommonResources, pCollisionManager);
	m_checkpoints.back()->GetTransform()->SetPosition(position);
}
