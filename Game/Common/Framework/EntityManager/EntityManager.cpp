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
 * @param[in] camera　カメラ
 */
void EntityManager::DrawEntity(const Camera& camera)
{
	UNREFERENCED_PARAMETER(camera);
	
}

/**
 * @brief 終了処理
 */
void EntityManager::Finalize()
{
}



