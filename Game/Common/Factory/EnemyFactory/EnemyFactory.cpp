/*****************************************************************//**
 * @file    EnemyFactory.h
 * @brief   敵の生成に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EnemyFactory.h"

#include "Game/GameObjects/Enemy/Enemy.h"
#include "Game/GameObjects/Enemy/Behaviour/Update/TrackingEnemyBehaviour/TrackingEnemyBehaviour.h"
#include "Game/GameObjects/Enemy/Behaviour/Update/FlyingChaserEnemyBehaviour/FlyingChaserEnemyBehaviour.h"

/**
 * @brief 追跡する敵の生成
 * 
 * @param[in] pPlayer
 * @return 
 */
std::unique_ptr<Enemy> EnemyFactory::TrackingEnemy::Create()
{
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();

	enemy->AddUpdateBehaviour(std::make_unique<TrackingEnemyBehaviour>());

	return std::move(enemy);
}

/**
 * @brief 空中追跡敵の生成
 * 
 * @return 
 */
std::unique_ptr<Enemy> EnemyFactory::FlyingChaserEnemy::Create()
{
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();

	enemy->AddUpdateBehaviour(std::make_unique<FlyingChaserEnemyBehaviour>());

	return std::move(enemy);
}
