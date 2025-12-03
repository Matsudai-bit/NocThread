/*****************************************************************//**
 * @file    BuildingManager.h
 * @brief   建物管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <iostream>
#include <fstream>
#include "BuildingManager.h"

#include "Game/Common/DeviceResources.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/DirectXFramework/ReadData.h"

#include "Game/Common/GameEffect/GameEffectController.h"
#include "Game/Common/GameEffect/Effects/SimpleParticle/SimpleParticle.h"
#include "Game/Common/Camera/MainCamera/MainCamera.h"

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


std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT = {
	// slot0 共通
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// slot1 インスタンス
		{ "WORLD",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "WORLD",    2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },


};

/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BuildingManager::Initialize(const CommonResources* pCommonResources)
{
	m_pCommonResources = pCommonResources;

	auto device = pCommonResources->GetDeviceResources()->GetD3DDevice();

	m_buildings.clear();
	GameEffectManager::EffectClip clip;
	clip.isLoop = true;
	GameEffectController::GetInstance()->PlayEffect(
		std::make_unique<SimpleParticle>(
			pCommonResources->GetDeviceResources(), DirectX::SimpleMath::Vector3::Zero, MainCamera::GetInstance()->GetCamera()), clip);
	
	// インスタンスバッファ生成
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(InstanceBuffer) * MAX_INSTANCE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		device->CreateBuffer(&bd, NULL, m_instanceBuffer.ReleaseAndGetAddressOf());
	}

	// 定数バッファの作成
	D3D11_BUFFER_DESC bufferDesc = {};
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer)
	);

	// 頂点シェーダの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/InatancedRender_VS.cso");
	
	DX::ThrowIfFailed(
		device->CreateVertexShader(vs.data(), vs.size(), nullptr, m_vs.ReleaseAndGetAddressOf())
	);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/InatancedRender_PS.cso");

	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
	);

	// インプットレイアウトの作成
	device->CreateInputLayout(
		&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		vs.data(), vs.size(),
		m_inputLayout.ReleaseAndGetAddressOf());

	

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
	// インスタング描画
	DrawInstance(view, projection);
	// 通常描画
	//DrawNormal(view, projection);

}

/**
 * @brief インスタンス描画
 * 
 * @param[in] view			ビュー行列
 * @param[in] projection	射影行列
 */
void BuildingManager::DrawInstance(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX;
	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	// 描画モデルの取得
	auto model = m_pCommonResources->GetResourceManager()->CreateModel("building.sdkmesh");


	D3D11_MAPPED_SUBRESOURCE msr{};

	// 作成するインスタンスの数
	int instanceCount = 0;
		
	// **** インスタンスバッファデータの設定 ****
	SetInstanceBuffer(m_instanceBuffer.Get(), context, msr, &instanceCount);

	// 頂点バッファの設定に必要な情報を取得
	const auto& meshParts = model.meshes.front()->meshParts.front();
	auto vertexBuffer = meshParts->vertexBuffer;
	auto vertexStride = meshParts->vertexStride;
	auto indexCount = meshParts->indexCount;

	// 頂点バッファを描画パイプラインに設定
	ID3D11Buffer* pBuf[2] = { vertexBuffer.Get(), m_instanceBuffer.Get() };
	UINT stride[2] = { vertexStride, sizeof(SimpleMath::Matrix) };
	UINT offset[2] = { 0, 0 };
	context->IASetVertexBuffers(0, 2, pBuf, stride, offset);

	// インデックスバッファの設定
	auto indexBuffer = meshParts->indexBuffer;
	context->IASetIndexBuffer(indexBuffer.Get(), meshParts->indexFormat, 0);

	// **** 定数バッファの設定 ****
	SetConstantBuffer(m_constantBuffer.Get(), context, msr, view, projection);
	// 定数バッファを頂点シェーダにバインド
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());


	// **** 各シェーダの設定 ****
	// 頂点シェーダの設定
	context->VSSetShader(m_vs.Get(), nullptr, 0);
	// ピクセルシェーダの設定
	context->PSSetShader(m_ps.Get(), nullptr, 0);

	// インプットレイアウトの設定
	context->IASetInputLayout(m_inputLayout.Get());
	//　ステンシルの設定
	context->OMSetDepthStencilState(m_pCommonResources->GetCommonStates()->DepthDefault(), 0);
	//	カリングはなし
	context->RSSetState(m_pCommonResources->GetCommonStates()->CullNone());
	// トポロジーの設定
	context->IASetPrimitiveTopology(meshParts->primitiveType);

	// インスタンス描画
	context->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
}

/**
 * @brief インスタンスバッファの設定
 * 
 * @param[out] pBuffer　バッファ
 * @param[in] context	
 * @param[in] msr
 * @param[out] pInstanceCount インスタンス数
 * @returns true	成功
 * @returns false	失敗
 */
bool BuildingManager::SetInstanceBuffer(ID3D11Buffer* pBuffer, ID3D11DeviceContext* context, D3D11_MAPPED_SUBRESOURCE msr, int* pInstanceCount)
{
	using namespace DirectX;

	if (pBuffer == nullptr ||
		context == nullptr)
	{
		return false;
	}

	int instanceCount = 0;

	// バッファに入れていく
	context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	{
		// データ場所設定・カウントリセット
		auto pInstanceData = (InstanceBuffer*)msr.pData;
		for (const auto& building : m_buildings)
		{
			SimpleMath::Matrix world = SimpleMath::Matrix::Identity;

			world *= SimpleMath::Matrix::CreateScale(building->GetExtends() * building->GetScale());
			world *= SimpleMath::Matrix::CreateFromQuaternion(building->GetRotate());
			world *= SimpleMath::Matrix::CreateTranslation(building->GetPosition());

			pInstanceData[instanceCount].world = world/*.Transpose()*/;
			instanceCount++;
		}
	}
	context->Unmap(pBuffer, 0);

	if (pInstanceCount)
	{
		*pInstanceCount = instanceCount;
	}
	return true;
}
/**
 * @brief 定数バッファの設定
 *
 * @param[out] pBuffer　バッファ
 * @param[in] context
 * @param[in] msr
 * @param[in] view
 * @param[in] projection
 * 
 * @returns true	成功
 * @returns false	失敗
 */
bool BuildingManager::SetConstantBuffer(ID3D11Buffer* pBuffer, ID3D11DeviceContext* context, D3D11_MAPPED_SUBRESOURCE msr, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{

	if (pBuffer == nullptr ||
		context == nullptr)
	{
		return false;
	}

	// 定数バッファに画面描画に必要な行列を設定する
	context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	{
		auto constantData = (ConstantBuffer*)msr.pData;

		constantData->viewProjMatrix = ((view) * (projection)).Transpose();
	}
	context->Unmap(pBuffer, 0);

	return true;
}

void BuildingManager::DrawNormal(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	//// 建物の描画処理
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

