/*****************************************************************//**
 * @file    BuildingManager.h
 * @brief   建物管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "BuildingManager.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/DirectXFramework/DebugDraw.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/DirectXFramework/ReadData.h"
#include "Library/MyLib/DirectXMyToolKit/DirectXUtils.h"

#include <fstream>
#include <iostream>
#include "Game/Common/Camera/Camera.h"

using json = nlohmann::json;
using namespace DirectX;

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
// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
BuildingManager::BuildingManager(const CommonResources* pCommonResources)
	: m_pCommonResources{ pCommonResources }
{
	m_buildings.clear();

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


	ID3D11Device1* device = m_pCommonResources->GetDeviceResources()->GetD3DDevice();
	// 定数バッファの作成
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags	= D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage		= D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(CameraFrustumConstantBuffer);
	desc.CPUAccessFlags =  D3D10_CPU_ACCESS_WRITE;
	;

	DX::ThrowIfFailed(
		device->CreateBuffer(&desc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));
	
	//バッファの生成
	MyLib::CreateStructuredBuffer(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, sizeof(ParticleCompute), static_cast<UINT>(334 * 10),m_particleBuffer.ReleaseAndGetAddressOf(), device);
	MyLib::CreateStructuredBuffer(D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, sizeof(ResultCompute), static_cast<UINT>(334 * 10), m_resultBuffer.ReleaseAndGetAddressOf(), device);
	MyLib::CreateStagingBuffer(m_resultBuffer.Get(), m_stagingBuffer.ReleaseAndGetAddressOf(), device);
	
	// SRVの生成
	MyLib::CreateBufferSRV(m_particleBuffer.Get(), m_particleSRV.ReleaseAndGetAddressOf(), device);

	// UAVの生成
	MyLib::CreateBufferUAV(m_resultBuffer.Get(), m_resultUAV.ReleaseAndGetAddressOf(), device);

	// コンピュートシェーダの作成
	std::vector<uint8_t> cs = DX::ReadData(L"Resources/Shaders/FrustumCullingCS.cso");
	DX::ThrowIfFailed(
		device->CreateComputeShader(cs.data(), cs.size(), nullptr, m_computeShader.ReleaseAndGetAddressOf())
	);
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool BuildingManager::UpdateTask(float deltaTime)
{
	// 建物の更新処理
	for (auto& building : m_buildings)
	{
		building->Update(deltaTime);
	}

	
	

	return true;
}

#include <chrono>

/**
 * @brief 描画処理
 * 
 * @param[in] camera	カメラ
 */
void BuildingManager::DrawTask(const Camera& camera)
{
	using namespace SimpleMath;
	
	// 1. 開始時刻の記録
	//auto start = std::chrono::high_resolution_clock::now();

	DrawDefault(camera);

	// 3. 終了時刻の記録
	//auto end = std::chrono::high_resolution_clock::now();

	// 4. 処理時間の計算と表示
	// duration_cast で希望の単位に変換（ここではマイクロ秒 ?s）
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	//m_pCommonResources->GetDebugFont()->AddString(100, 100, Colors::Red, L"duration(?s) = %d", duration.count());

	
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
				saveData.tileNumber,
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
 * @param[in] tileNumber		タイル番号
 * @param[in] pCollisionManager	衝突管理
 * @param[in] pCommonResources	共通リソース
 */
void BuildingManager::CreateBuilding(
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Vector3& scale,
	const int& tileNumber,
	CollisionManager* pCollisionManager,
	const CommonResources* pCommonResources)
{


	auto building = std::make_unique<Building>();

	building->GetTransform()->SetPosition(position);
	building->SetExtraScale(scale);
	building->Initialize(pCommonResources, pCollisionManager);
	building->SetTileNumber(tileNumber);

	m_buildings.emplace_back(std::move(building));
}

/**
 * @brief 建物を探す
 * 
 * @param[in]	tileNumber　タイル番号
 * @param[out]	outBuilding 取得建物
 * 
 * @returns true 見つけた
 * @returns false見つからなかった
 */
bool BuildingManager::FindBuilding(const int& tileNumber, const Building*& outBuilding) const
{
	// 検索する
	auto it =  std::find_if(m_buildings.begin(), m_buildings.end(), [tileNumber](const std::unique_ptr<Building>& building)
		{
			return (tileNumber == building->GetTileNumber());
		});

	if (it == m_buildings.end())
	{
		return false;
	}

	outBuilding = it->get();
	return true;
}



void BuildingManager::DrawDefault(const Camera& camera)
{
	// 建物の描画処理
	for (auto& building : m_buildings)
	{

			building->Draw(camera);
	}
}

void BuildingManager::DrawFrustumCulling(const Camera& camera)
{
	const auto cameraFrustum = camera.CalcFrustum();

	// 建物の描画処理
	for (auto& building : m_buildings)
	{

		// 最初に描画を有効と仮定し、平面の外側に完全に出たら描画を無効にする
		auto cullingSphere = DirectX::BoundingSphere(
			building->GetCullingCollider()->GetPosition(),
			building->GetCullingCollider()->GetRadius());

		ContainmentType result = cameraFrustum.Contains(cullingSphere);
		// DISJOINT (完全に外側) でない場合、描画が必要
		if (result != DISJOINT)
		{
			building->Draw(camera);
		}


	}
}

void BuildingManager::DrawFrustumCullingCS(const Camera& camera)
{
	using namespace SimpleMath;
	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	UINT buildingCount = static_cast<UINT>(m_buildings.size());

	const auto cameraFrustum = camera.CalcFrustum();
	const int PLANE_NUM = 6;

	// 1. 定数バッファの更新 (平面情報 + 最大数)
	{
		DirectX::XMVECTOR planesM[PLANE_NUM];
		cameraFrustum.GetPlanes(&planesM[0], &planesM[1], &planesM[2], &planesM[3], &planesM[4], &planesM[5]);

		CameraFrustumConstantBuffer constantData{};
		for (int j = 0; j < PLANE_NUM; j++) {
			constantData.planes[j] = Vector4(planesM[j]);
		}
		constantData.maxIndexCount = buildingCount; // ★ガード用のカウント

		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes))) {
			memcpy(subRes.pData, &constantData, sizeof(CameraFrustumConstantBuffer));
			context->Unmap(m_constantBuffer.Get(), 0);
		}
	}

	// 2. 建物データの更新 (位置 + 半径)
	{
		m_particles.clear();
		for (auto& building : m_buildings) {
			ParticleCompute data;
			data.position = building->GetCullingCollider()->GetPosition();
			data.radius = building->GetCullingCollider()->GetRadius();
			m_particles.emplace_back(data);
		}

		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(context->Map(m_particleBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes))) {
			memcpy(subRes.pData, m_particles.data(), sizeof(ParticleCompute) * m_particles.size());
			context->Unmap(m_particleBuffer.Get(), 0);
		}
	}

	//　コンピュートシェーダー実行
	ID3D11ShaderResourceView* pSRVs[1] = {m_particleSRV.Get() };
	ID3D11Buffer* pBuff[1] = { m_constantBuffer.Get() };
	context->CSSetConstantBuffers(0, 1, pBuff);
	context->CSSetShaderResources(0, 1, pSRVs);
	context->CSSetShader(m_computeShader.Get(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, m_resultUAV.GetAddressOf(), 0);
	UINT threadGroupsX = (static_cast<UINT>(m_buildings.size()) + 255) / 256;
	context->Dispatch(threadGroupsX, 1, 1);

	// uavの再設定によるリソースバリア
	ID3D11UnorderedAccessView* nulluav = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &nulluav, nullptr);
	std::vector<ResultCompute> cullingResults(m_particles.size());
	// コンピュートシェーダーの結果を取得
	context->CopyResource(m_stagingBuffer.Get(), m_resultBuffer.Get());
	D3D11_MAPPED_SUBRESOURCE subRes{};
	if (SUCCEEDED(context->Map(m_stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &subRes)))
	{
		auto pResultData = reinterpret_cast<ResultCompute*>(subRes.pData);

		// 5. 結果を走査して描画
		for (UINT i = 0; i < buildingCount; ++i)
		{
			if (pResultData[i].index != -1)
			{
				m_buildings[i]->Draw(camera);
			}
		}

		// 使い終わってから Unmap する
		context->Unmap(m_stagingBuffer.Get(), 0);
	}
}

void BuildingManager::Save()
{

	std::vector<BuildingSaveData> saves(m_buildings.size());

	for (int i = 0; i < m_buildings.size(); i++)
	{
		// Vector3Data は GetPosition() などの型とは別なので、手動で値をコピー
		saves[i].position.x = m_buildings[i]->GetTransform()->GetPosition().x;
		saves[i].position.y = m_buildings[i]->GetTransform()->GetPosition().y;
		saves[i].position.z = m_buildings[i]->GetTransform()->GetPosition().z;

		saves[i].scale.x = m_buildings[i]->GetExtraScale().x;
		saves[i].scale.y = m_buildings[i]->GetExtraScale().y;
		saves[i].scale.z = m_buildings[i]->GetExtraScale().z;
	}

	// ★★★ to_json が自動で呼び出され、JSON配列 j が生成される ★★★
	nlohmann::json j = saves;

	// 生成されたJSONを整形してファイルに出力する（追加の処理）
	 std::ofstream ofs("Resources/buildings.json");
	 ofs << j.dump(4);
}

