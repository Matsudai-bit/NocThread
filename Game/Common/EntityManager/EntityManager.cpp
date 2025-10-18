/*****************************************************************//**
 * @file    EntityManager.h
 * @brief   エンティティ管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EntityManager.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
EntityManager::EntityManager()
{

}



/**
 * @brief デストラクタ
 */
EntityManager::~EntityManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EntityManager::Initialize()
{

}

/**
 * @brief エンティティの更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void EntityManager::UpdateEntity(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

/**
 * @brief エンティティの描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 */
void EntityManager::DrawEntity(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	UNREFERENCED_PARAMETER(view);
	UNREFERENCED_PARAMETER(proj);
}

/**
 * @brief 終了処理
 */
void EntityManager::Finalize()
{
}



