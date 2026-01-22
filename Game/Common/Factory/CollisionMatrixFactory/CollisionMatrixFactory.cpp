/*****************************************************************//**
 * @file    PlayerFactory.h
 * @brief   敵の生成に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionMatrixFactory.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/CollisionMatrix/CollisionMatrix.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/GameObject.h"


using namespace DirectX;

/**
 * @brief オブジェクトを組み立てる
 * 
 * @param[in] instance　生成されたオブジェクト
 */
void CollisionMatrixFactory::StageCollisionMatrix::Assemble(CollisionMatrix* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(desc);

	instance->ClearMatrix();

	// 登録する
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::BUILDING);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::ENEMY);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::ESCAPE_HELICOPTER);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::CHECKPOINT);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::TREASURE);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::STAGE_OBJECT);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::FLOOR);
	instance->RegisterDetectionTarget(GameObjectTag::PLAYER, GameObjectTag::DEFAULT);

	instance->RegisterDetectionTarget(GameObjectTag::ENEMY, GameObjectTag::FLOOR);
	instance->RegisterDetectionTarget(GameObjectTag::ENEMY, GameObjectTag::PLAYER);
	instance->RegisterDetectionTarget(GameObjectTag::ENEMY, GameObjectTag::BUILDING);
	instance->RegisterDetectionTarget(GameObjectTag::ENEMY, GameObjectTag::CHECKPOINT);
	instance->RegisterDetectionTarget(GameObjectTag::ENEMY, GameObjectTag::ESCAPE_HELICOPTER);
	instance->RegisterDetectionTarget(GameObjectTag::ENEMY, GameObjectTag::DEFAULT);

	instance->RegisterDetectionTarget(GameObjectTag::WIRE_GRAPPING_AREA, GameObjectTag::ESCAPE_HELICOPTER);
	instance->RegisterDetectionTarget(GameObjectTag::WIRE_GRAPPING_AREA, GameObjectTag::BUILDING);
	instance->RegisterDetectionTarget(GameObjectTag::WIRE_GRAPPING_AREA, GameObjectTag::ENEMY);
	instance->RegisterDetectionTarget(GameObjectTag::WIRE_GRAPPING_AREA, GameObjectTag::DEFAULT);

	instance->RegisterDetectionTarget(GameObjectTag::WIRE, GameObjectTag::ESCAPE_HELICOPTER);
	instance->RegisterDetectionTarget(GameObjectTag::WIRE, GameObjectTag::BUILDING);
	instance->RegisterDetectionTarget(GameObjectTag::WIRE, GameObjectTag::ENEMY);

	instance->RegisterDetectionTarget(GameObjectTag::CAMERA, GameObjectTag::BUILDING);
	instance->RegisterDetectionTarget(GameObjectTag::CAMERA, GameObjectTag::ESCAPE_HELICOPTER);
	instance->RegisterDetectionTarget(GameObjectTag::CAMERA, GameObjectTag::DEFAULT);
}
