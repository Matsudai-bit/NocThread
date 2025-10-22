/*****************************************************************//**
 * @file    BuildingManager.h
 * @brief   建物管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <fstream>
#include "BuildingManager.h"
#include <iostream>

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
		{"Scale", s.scale}
	};
}

void from_json(const json& j, BuildingSaveData& s)
{
	j.at("Position").get_to(s.position);
	j.at("Scale").get_to(s.scale);
}
// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
BuildingManager::BuildingManager()
{

}



/**
 * @brief デストラクタ
 */
BuildingManager::~BuildingManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BuildingManager::Initialize()
{
	m_buildings.clear();


}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void BuildingManager::Update(float deltaTime)
{
	// 建物の更新処理
	for (auto& building : m_buildings)
	{
		building->Update(deltaTime);
	}
}



/**
 * @brief 描画処理
 * 
 * @param[in] view			ビュー行列
 * @param[in] projection	射影行列
 */
void BuildingManager::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	// 建物の描画処理
	for (auto& building : m_buildings)
	{
		building->Draw(view, projection);
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BuildingManager::Finalize()
{

}

bool BuildingManager::RequestCreate(CollisionManager* pCollisionManager, const CommonResources* pCommonResources)
{
	using namespace DirectX;
	using namespace SimpleMath;

	const std::string& filename = "Resources/buildings.json";

	// 既存の建物をクリア
	m_buildings.clear();

	std::ifstream ifs(filename);
	if (!ifs.is_open())
	{
		std::cerr << "Error: BuildingManager::Load - Could not open file " << filename << std::endl;
		return false;
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
			return false;
		}


		// データ構造をループし、CreateBuildingで生成
		for (const auto& saveData : buildingSaves)
		{
			// BuildingSaveData (Vector3Data) から DirectX::SimpleMath::Vector3 へ変換
			Vector3 position(saveData.position.x, saveData.position.y, saveData.position.z);
			Vector3 scale(saveData.scale.x, saveData.scale.y, saveData.scale.z);

			// 既存の CreateBuilding 関数を呼び出し
			CreateBuilding(
				position,
				scale,
				pCollisionManager,
				pCommonResources
			);
		}

		std::cout << "Successfully loaded and created " << buildingSaves.size() << " buildings." << std::endl;
		return true;
	}
	catch (const json::exception& e)
	{
		std::cerr << "JSON Error during loading (" << filename << "): " << e.what() << std::endl;
		return false;
	}
}


/**
 * @brief 建物の作成
 * 
 * @param[in] position　		座標
 * @param[in] scale				拡大率
 * @param[in] pCollisionManager	衝突管理
 * @param[in] pCommonResources	共通リソース
 */
void BuildingManager::CreateBuilding(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Vector3& scale,
	CollisionManager* pCollisionManager,
	const CommonResources* pCommonResources)
{


	auto building = std::make_unique<Building>();

	building->SetPosition(position);
	building->SetExtends(scale);
	building->Initialize(pCommonResources, pCollisionManager);

	m_buildings.emplace_back(std::move(building));
}

void BuildingManager::Save()
{

	std::vector<BuildingSaveData> saves(m_buildings.size());

	for (int i = 0; i < m_buildings.size(); i++)
	{
		// Vector3Data は GetPosition() などの型とは別なので、手動で値をコピー
		saves[i].position.x = m_buildings[i]->GetPosition().x;
		saves[i].position.y = m_buildings[i]->GetPosition().y;
		saves[i].position.z = m_buildings[i]->GetPosition().z;

		saves[i].scale.x = m_buildings[i]->GetExtends().x;
		saves[i].scale.y = m_buildings[i]->GetExtends().y;
		saves[i].scale.z = m_buildings[i]->GetExtends().z;
	}

	// ★★★ to_json が自動で呼び出され、JSON配列 j が生成される ★★★
	nlohmann::json j = saves;

	// 生成されたJSONを整形してファイルに出力する（追加の処理）
	 std::ofstream ofs("Resources/buildings.json");
	 ofs << j.dump(4);
}

