/*****************************************************************//**
 * @file    BuildingFactory.h
 * @brief   敵の生成に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "Game/GameObjects/Prop/Building/Building.h"
#include "BuildingFactory.h"


void TownFactory::Assemble(std::vector<Building>* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);
}

void FlyingChaserBuilding::Assemble(Building* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);
}
