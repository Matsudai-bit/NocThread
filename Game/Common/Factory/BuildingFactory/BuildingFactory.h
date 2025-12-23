/*****************************************************************//**
 * @file    BuildingFactory.h
 * @brief   建物の生成に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>
#include "Game/Common/Factory/FactoryBase.h"


// クラスの前方宣言 ===================================================
class Building;
class Player;


/**
* @brief 街ファクトリー
*/
class TownFactory
	: public FactoryBase<std::vector<Building>>
{
public:
	// コンストラクタ
	TownFactory() = default;

	// デストラクタ
	~TownFactory() = default;

public:


	// 生成したオブジェクトを組み立てる
	void Assemble(std::vector<Building>* instance, const DefaultSpawnDesc& desc) override;
};

/**
* @brief 建物ファクトリー
*/
class FlyingChaserBuilding
	: public FactoryBase<Building>
{
public:
	// コンストラクタ
	FlyingChaserBuilding() = default;

	// デストラクタ
	~FlyingChaserBuilding() = default;

public:
	// 生成したオブジェクトを組み立てる
	void Assemble(Building* instance, const DefaultSpawnDesc& desc) override;

};


