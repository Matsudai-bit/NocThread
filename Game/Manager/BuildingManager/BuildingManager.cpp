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


	ID3D11Device1* device = m_pCommonResources->GetDeviceResources()->GetD3DDevice();
	//バッファの生成
	CreateStructuredBuffer(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, sizeof(ParticleCompute), static_cast<UINT>(DEFAULT_BUFFER_SIZE), &m_pParticleBuffer, device);
	CreateStructuredBuffer(D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, sizeof(ResultCompute), static_cast<UINT>(DEFAULT_BUFFER_SIZE), &m_pResultBuffer, device);
	
	// SRVの生成
	CreateBufferSRV(m_pParticleBuffer, &m_pParticleSRV, device);

	// UAVの生成
	CreateBufferUAV(m_pResultBuffer, &m_pResultUAV, device);

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
	auto start = std::chrono::high_resolution_clock::now();

	DrawCS(camera);

	// 3. 終了時刻の記録
	auto end = std::chrono::high_resolution_clock::now();

	// 4. 処理時間の計算と表示
	// duration_cast で希望の単位に変換（ここではマイクロ秒 ?s）
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

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

void BuildingManager::CreateStructuredBuffer(UINT BindFlags, D3D11_USAGE Usage, UINT elementSize, UINT count, ID3D11Buffer** ppBuffer, ID3D11Device1* device)
{
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags		= BindFlags;
	desc.Usage			= Usage;
	desc.ByteWidth		= elementSize * count;
	desc.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(
		device->CreateBuffer(&desc, nullptr, ppBuffer));
}

void BuildingManager::CreateBufferSRV(ID3D11Buffer* pSourceBuffer, ID3D11ShaderResourceView** ppSRView, ID3D11Device1* device)
{
	if (!pSourceBuffer || !ppSRView || !device) { return; }


	D3D11_BUFFER_DESC sourceBufferDesc{};
	pSourceBuffer->GetDesc(&sourceBufferDesc);

	// SRVのStructuredBuffer であることを確認
	if (!(sourceBufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) ||
		sourceBufferDesc.StructureByteStride == 0 ||
		sourceBufferDesc.BindFlags != D3D11_BIND_SHADER_RESOURCE) {return;}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.ViewDimension	= D3D11_SRV_DIMENSION_BUFFER;
	desc.Format			= DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements		= sourceBufferDesc.ByteWidth / sourceBufferDesc.StructureByteStride;
	desc.Buffer.FirstElement	= 0;
	
	DX::ThrowIfFailed(
		device->CreateShaderResourceView(pSourceBuffer, &desc, ppSRView));
}

void BuildingManager::CreateBufferUAV(ID3D11Buffer* pSourceBuffer, ID3D11UnorderedAccessView** ppUAView, ID3D11Device1* device)
{
	if (!pSourceBuffer || !ppUAView || !device) { return; }


	D3D11_BUFFER_DESC sourceBufferDesc{};
	pSourceBuffer->GetDesc(&sourceBufferDesc);

	// UAVのStructuredBuffer であることを確認
	if (!(sourceBufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) ||
		sourceBufferDesc.StructureByteStride == 0 ||
		sourceBufferDesc.BindFlags != D3D11_BIND_UNORDERED_ACCESS) {	return;	}


	D3D11_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = sourceBufferDesc.ByteWidth / sourceBufferDesc.StructureByteStride;
	desc.Buffer.FirstElement = 0;


	DX::ThrowIfFailed(
		device->CreateUnorderedAccessView(pSourceBuffer, &desc, ppUAView));
}

ID3D11Buffer* BuildingManager::CreateAndCopyToBuffer(ID3D11Buffer* pSourceBuffer, ID3D11Device1* device, ID3D11DeviceContext* context)
{
	// ソースバッファの情報を取得
	D3D11_BUFFER_DESC sourceDesc;
	pSourceBuffer->GetDesc(&sourceDesc);

	// ステージングバッファの作成
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth	= sourceDesc.ByteWidth;	// 元と同じサイズ
	desc.Usage		= D3D11_USAGE_STAGING;	// CPU転送用の設定
	desc.BindFlags	= 0;					// パイプラインにバインドしない
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;   // CPUでの読み取りを許可
	desc.MiscFlags	= 0;

	ID3D11Buffer* pStagingBuffer;

	DX::ThrowIfFailed(
		device->CreateBuffer(&desc, nullptr, &pStagingBuffer));

	// ソース元のバッファの中身を一括コピー
	context->CopyResource(pStagingBuffer, pSourceBuffer);

	return pStagingBuffer;
}

void BuildingManager::DrawDefault(const Camera& camera)
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

void BuildingManager::DrawCS(const Camera& camera)
{
	using namespace SimpleMath;

	const auto cameraFrustum = camera.CalcFrustum();
	const int PLANE_NUM = 6;

	//**** 送るデータの作成 ****
	{
		m_particles.clear();
		// 6つの平面を受け取る配列
		DirectX::XMVECTOR planesM[PLANE_NUM];
		// 平面を取得
		cameraFrustum.GetPlanes(&planesM[0], &planesM[1], &planesM[2], &planesM[3], &planesM[4], &planesM[5]);


		for (size_t i = 0; i < m_buildings.size(); i++)
		{
			ParticleCompute sendingData;

			for (int j = 0; j < PLANE_NUM; j++)
			{
				sendingData.planes[j] = Vector4(planesM[j]);
			}
			sendingData.position = m_buildings[i]->GetCullingCollider()->GetPosition();
			sendingData.radius = m_buildings[i]->GetCullingCollider()->GetRadius();

			m_particles.emplace_back(sendingData);
		}
	}



	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = m_pCommonResources->GetDeviceResources()->GetD3DDevice();
	// パーティクルの資料をバッファに入れる
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		context->Map(m_pParticleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
		auto pBufType = (ParticleCompute*)subRes.pData;
		memcpy(subRes.pData, m_particles.data(), sizeof(ParticleCompute) * m_particles.size());
		context->Unmap(m_pParticleBuffer, 0);
	}

	//　コンピュートシェーダー実行
	ID3D11ShaderResourceView* pSRVs[1] = { m_pParticleSRV };
	context->CSSetShaderResources(0, 1, pSRVs);
	context->CSSetShader(m_computeShader.Get(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, &m_pResultUAV, 0);
	context->Dispatch(256, 1, 1);

	// uavの再設定によるリソースバリア
	ID3D11UnorderedAccessView* nulluav = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &nulluav, nullptr);
	std::vector<ResultCompute> cullingResults(m_particles.size());
	// コンピュートシェーダーの結果を取得
	{
		auto stagingBuffer = CreateAndCopyToBuffer(m_pResultBuffer, device, context);
		D3D11_MAPPED_SUBRESOURCE subRes{};
		context->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &subRes);
		auto pResultData = (ResultCompute*)subRes.pData;
		memcpy(cullingResults.data(), pResultData, sizeof(ResultCompute) * cullingResults.size());
		context->Unmap(stagingBuffer, 0);
	}

	cullingResults.erase(std::remove_if(cullingResults.begin(), cullingResults.end(),
		[&](const ResultCompute& result)
		{
			return (result.index == -1);
		}
	), cullingResults.end());

	for (auto& result : cullingResults)
	{
		m_buildings[result.index]->Draw(camera);
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

