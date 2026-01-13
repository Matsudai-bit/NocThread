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
class CollisionManager;
class CommonResources;

struct TownFactoryDesk
{
	CollisionManager* pCollisionManager;
	const CommonResources& commonResources;
	std::string jsonFilePath;

	TownFactoryDesk(
		CollisionManager* pCollisionManager,
		const CommonResources& commonResources,
		const std::string& jsonFilePath)
		: pCollisionManager{ pCollisionManager }
		, commonResources{ commonResources }
		, jsonFilePath{ jsonFilePath }
	{
	}
};
/**
* @brief 街ファクトリー
*/
class TownFactory
	: public FactoryBase<std::vector<std::unique_ptr<Building>>, TownFactoryDesk>
{
// 構造体
public:


public:
	// コンストラクタ
	TownFactory() = default;

	// デストラクタ
	~TownFactory() = default;

public:


	// 生成したオブジェクトを組み立てる
	void Assemble(std::vector<std::unique_ptr<Building>>* instance, const TownFactoryDesk& desc) override;

private:
	
};

// 構造体
struct BuildingFactoryDesc
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 scale;
	int tileNumber;
	CollisionManager* pCollisionManager;
	const CommonResources& pCommonResources;

	BuildingFactoryDesc(
		const DirectX::SimpleMath::Vector3& position,
		const DirectX::SimpleMath::Vector3& scale,
		int tileNumber,
		CollisionManager* pCollisionManager,
		const CommonResources& commonResources)
		: position{ position }
		, scale{ scale }
		, tileNumber{ tileNumber }
		, pCollisionManager{ pCollisionManager }
		, pCommonResources{ commonResources }
	{
	}

};
/**
* @brief 建物ファクトリー
*/
class BuildingFactory	
	: public FactoryBase<Building, BuildingFactoryDesc>
{
public:


public:
	// コンストラクタ
	BuildingFactory() = default;

	// デストラクタ
	~BuildingFactory() = default;

public:
	// 生成したオブジェクトを組み立てる
	void Assemble(Building* instance, const BuildingFactoryDesc& desc) override;

};


