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
 * @brief オブジェクトを組み立てる
 * 
 * @param[in] instance　生成されたオブジェクト
 */
void EnemyFactory::TrackingEnemy::Assemble(Enemy* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(desc);
	instance->AddUpdateBehaviour(std::make_unique<FlyingChaserEnemyBehaviour>());

}

void EnemyFactory::FlyingChaserEnemy::Assemble(Enemy* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(desc);
	instance->AddUpdateBehaviour(std::make_unique<FlyingChaserEnemyBehaviour>());

}
