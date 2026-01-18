/*****************************************************************//**
 * @file    PlayerFactory.h
 * @brief   敵の生成に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PlayerFactory.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Prop/Building/Building.h"
#include "Game/Manager/BuildingManager/BuildingManager.h"

using namespace DirectX;

/**
 * @brief オブジェクトを組み立てる
 * 
 * @param[in] instance　生成されたオブジェクト
 */
void PlayerFactory::StagePlayer::Assemble(Player* instance, const PlayerDesk& desc)
{
	// 初期化処理
	instance->Initialize(&desc.commonResources, desc.pCollisionManager, desc.pPlayerCamera, desc.pPlayerInput);

	// 出現位置の建物を取得する
	const Building* tileBuilding = nullptr;
	if (desc.buildingManager.FindBuilding(desc.tileNumber, tileBuilding))
	{
		// 建物の上に立つように接地
		instance->GetTransform()->SetPosition(tileBuilding->GetTransform()->GetPosition() + SimpleMath::Vector3(0.0f, 80.0f, 0.0f));
	}
}
