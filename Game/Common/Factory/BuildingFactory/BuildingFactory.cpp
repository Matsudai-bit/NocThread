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
#include <fstream>
#include <iostream>
#include <nlohmann\json.hpp>

using json = nlohmann::json;

// -------------------------------------------------------------------
// 1. データ構造の定義 (BuildingManagerクラスの外へ移動)
// -------------------------------------------------------------------

struct Vector3Data
{
	float x;
	float y;
	float z;
};

struct BuildingSaveData // 名前をSaveDataから変更して衝突を避ける
{
	Vector3Data position;
	Vector3Data scale;
	int tileNumber;
};

// -------------------------------------------------------------------
// 2. JSON 自動変換関数の定義 (構造体の外、グローバルスコープ)
// -------------------------------------------------------------------

// Vector3Data の変換 ---------------------------------------
void to_json(json& j, const Vector3Data& p)
{
	j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
}

void from_json(const json& j, Vector3Data& p)
{
	j.at("x").get_to(p.x);
	j.at("y").get_to(p.y);
	j.at("z").get_to(p.z);
}

// BuildingSaveData の変換 ---------------------------------------
void to_json(json& j, const BuildingSaveData& s)
{
	j = json{
		{"Position", s.position},
		{"Scale", s.scale},
		{"TileNumber", s.tileNumber}
	};
}

void from_json(const json& j, BuildingSaveData& s)
{
	j.at("Position").get_to(s.position);
	j.at("Scale").get_to(s.scale);
	j.at("TileNumber").get_to(s.tileNumber);
}

void TownFactory::Assemble(std::vector<std::unique_ptr<Building>>* instance, const TownFactoryDesk& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);

	using namespace DirectX;
	using namespace SimpleMath;

	const std::string& filename = desc.jsonFilePath;

	
	std::ifstream ifs(filename);
	if (!ifs.is_open())
	{
		std::cerr << "Error: BuildingManager::Load - Could not open file " << filename << std::endl;
		return ;
	}

	json j;
	try
	{
		// ファイルからJSONオブジェクトへ読み込み
		ifs >> j;



		// JSONオブジェクトからC++のデータ構造へ変換 (StageData を前提とする)
		// 以前のJSON構造に合わせて、BuildingSaveDataのvectorに直接変換する
		std::vector<BuildingSaveData> buildingSaves;

		// JSONの最上位が配列の場合、そのまま読み込み
		// ※もし {"Buildings": [...] } の形式なら j.at("Buildings").get_to(buildingSaves); に変更
		if (j.is_array())
		{
			buildingSaves = j.get<std::vector<BuildingSaveData>>();
		}
		else
		{
			std::cerr << "Error: JSON root is not an array." << std::endl;
			return ;
		}

		BuildingFactory buildingFactory;
		// データ構造をループし、CreateBuildingで生成
		for (const auto& saveData : buildingSaves)
		{
			// BuildingSaveData (Vector3Data) から DirectX::SimpleMath::Vector3 へ変換
			Vector3 position(saveData.position.x, saveData.position.y, saveData.position.z);
			Vector3 scale(saveData.scale.x, saveData.scale.y, saveData.scale.z);


			// 既存の CreateBuilding 関数を呼び出し
			BuildingFactoryDesc factoryDesc{position, scale, saveData.tileNumber,desc.pCollisionManager, desc.commonResources};
			
			instance->emplace_back(buildingFactory.Create(factoryDesc));




		}

		std::cout << "Successfully loaded and created " << buildingSaves.size() << " buildings." << std::endl;
		return ;
	}
	catch (const json::exception& e)
	{
		std::cerr << "JSON Error during loading (" << filename << "): " << e.what() << std::endl;
		return ;
	}
}



void BuildingFactory::Assemble(Building* instance, const BuildingFactoryDesc& desc)
{

	instance->GetTransform()->SetPosition(desc.position);
	instance->SetExtraScale(desc.scale);
	instance->Initialize(&desc.pCommonResources, desc.pCollisionManager);
	instance->SetTileNumber(desc.tileNumber);

}
