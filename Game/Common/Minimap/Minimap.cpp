/*****************************************************************//**
 * @file    Minimap.h
 * @brief   ミニマップに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/11/23
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include <vector>

#include "pch.h"
#include "Minimap.h"

// DirectXTK系
#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/DirectXFramework/DeviceResources.h"

// ライブラリ
#include "Library/DirectXFramework/ReadData.h"

// 描画関連
#include "Game/Common/Camera/Camera.h"
#include "Game/Common/Screen.h"

#include "Game/Common/GameObjectRegistry/GameObjectRegistry.h"

using namespace DirectX;

///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> Minimap::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },

};
std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT_INSTANCING = {
	// slot0 共通
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		

		// slot1 インスタンス
		{ "WORLD",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "WORLD",    2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

		{ "COLOR",    1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },




};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pCommonResources 共通リソース
 */
Minimap::Minimap(const CommonResources* pCommonResources)
	: m_pCommonResources{ pCommonResources }
{
	auto device = pCommonResources->GetDeviceResources()->GetD3DDevice();
	auto context = pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/SimpleCircle_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps_Circle.ReleaseAndGetAddressOf())
	);

	ps = DX::ReadData(L"Resources/Shaders/SimpleRectangle_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps_Rectangle.ReleaseAndGetAddressOf())	
	);

	ps = DX::ReadData(L"Resources/Shaders/SimpleTriangle_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps_Triangle.ReleaseAndGetAddressOf())
	);

	// ジオメトリシェーダの作成
	std::vector<uint8_t> gs = DX::ReadData(L"Resources/Shaders/SimpleShape2D_GS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(gs.data(), gs.size(), nullptr, m_gs.ReleaseAndGetAddressOf())
	);

	// バーテックスの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/SimpleShape2D_VS.cso");
	DX::ThrowIfFailed(
		device->CreateVertexShader(vs.data(), vs.size(), nullptr, m_vs.ReleaseAndGetAddressOf())
	);

	device->CreateInputLayout(
		&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		vs.data(), vs.size(),
		m_inputLayout.ReleaseAndGetAddressOf());

	// 定数バッファの作成
	D3D11_BUFFER_DESC bufferDesc = {};
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer)
	);
	 
	//m_offscreenRendering = std::make_unique<MyLib::OffscreenRendering>

	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	SimpleMath::Vector2 m_mapSize = RENDERING_SIZE * Screen::Get()->GetScreenScale();
	RECT rect{
		0,
		0,
		static_cast<LONG>(m_mapSize.x),
		static_cast<LONG>(m_mapSize.y)

	};

	// オフスクリーンレンダリングの作成
	m_offscreenRendering = std::make_unique<MyLib::OffscreenRendering>(
		pCommonResources->GetDeviceResources(),
		pCommonResources->GetDeviceResources()->GetBackBufferFormat(),
		pCommonResources->GetDeviceResources()->GetDepthBufferFormat());
	m_offscreenRendering->SetDevice(device);
	m_offscreenRendering->SetWindow(rect);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps_minimap = DX::ReadData(L"Resources/Shaders/Minimap_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_minimap.data(), ps_minimap.size(), nullptr, m_PS_Minimap.ReleaseAndGetAddressOf())
	);
	D3D11_BUFFER_DESC bufferDesk = {};
	bufferDesk.ByteWidth = static_cast<UINT>(sizeof(MinimapConstantBuffer));
	bufferDesk.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesk.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesk.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesk, nullptr, m_minimapConstantBuffer.ReleaseAndGetAddressOf())
	);

	// ミニマップ用スプライトの作成
	m_minimapSprite = std::make_unique<Sprite>();

	SetUpInstancing();
}



/**
 * @brief デストラクタ
 */
Minimap::~Minimap()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Minimap::Initialize()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool Minimap::UpdateTask(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);


	m_mapSize = RENDERING_SIZE * Screen::Get()->GetScreenScale();

	return true;
}

/**
 * @brief 描画処理
 *
 * @param[in] camera カメラ
 *
 * @return なし
 */
void Minimap::DrawTask(const Camera& camera)
{
	UNREFERENCED_PARAMETER(camera);

	using namespace SimpleMath;
	// ミニマップテクスチャの作成
	CreateMinimapTexture();

	// 処理用変数
	auto states = m_pCommonResources->GetCommonStates();
	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	auto outputSize = m_pCommonResources->GetDeviceResources()->GetOutputSize();
	auto mapSize = SimpleMath::Vector2(outputSize.right, outputSize.bottom);

	RECT rect{
		0,
		0,
		static_cast<LONG>(mapSize.x),
		static_cast<LONG>(mapSize.y)

	};

	m_minimapSprite->Initialize(m_offscreenRendering->GetShaderResourceView());
	m_minimapSprite->SetScale(MAP_SCALE * Screen::Get()->GetScreenScale());
	m_minimapSprite->SetPosition(MAP_POSITION * Screen::Get()->GetScreenScale());
	m_minimapSprite->SetRegion(rect);

	// ミニマップの描画
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(m_minimapConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto data = static_cast<MinimapConstantBuffer*>(mappedResource.pData);
	data->transparentColor = MINIMAP_BACK_COLOR;
	data->aspect = mapSize;
	context->Unmap(m_minimapConstantBuffer.Get(), 0);

	m_spriteBatch->Begin(
		SpriteSortMode_Immediate, states->NonPremultiplied(), nullptr, nullptr, nullptr,
		[&]() {

			// 定数バッファの作成
			ID3D11Buffer* cbuff = { m_minimapConstantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, &cbuff);

			// ピクセルシェーダ
			context->PSSetShader(m_PS_Minimap.Get(), nullptr, 0);
		}
	);	
	{
		m_minimapSprite->DrawSprite(m_spriteBatch.get());
	}
	m_spriteBatch->End();

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Minimap::Finalize()
{

}




int MAX_INSTANCE = 1000;

void Minimap::SetUpInstancing()
{
	auto device = m_pCommonResources->GetDeviceResources()->GetD3DDevice();

	// インスタンスバッファ生成
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(InstanceBuffer) * MAX_INSTANCE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		device->CreateBuffer(&bd, NULL, m_instanceBufferIns.ReleaseAndGetAddressOf());
	}

	// 定数バッファの作成
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		DX::ThrowIfFailed(
			device->CreateBuffer(&bufferDesc, nullptr, &m_constantBufferIns)
		);

	}

	// 頂点バッファの作成
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth		= static_cast<UINT>(sizeof(VertexData)) * 4;
		bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags		= 0;
		DX::ThrowIfFailed(
			device->CreateBuffer(&bufferDesc, nullptr, m_vertexBufferIns.ReleaseAndGetAddressOf())
		);
	}

	uint16_t indices[] = 
	{
		0,1,2,
		2,1,3
	};
	// インデックスバッファの作成
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth	= static_cast<UINT>(sizeof(uint16_t)) * 6;
		bufferDesc.Usage		= D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags= 0;
		bufferDesc.MiscFlags	= 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = indices;
		DX::ThrowIfFailed(
			device->CreateBuffer(&bufferDesc, &initData, m_indexBufferIns.ReleaseAndGetAddressOf())
		);
	}




	// ジオメトリシェーダの作成
	std::vector<uint8_t> gs = DX::ReadData(L"Resources/Shaders/SimpleShape2DInstansing_GS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(gs.data(), gs.size(), nullptr, m_gsIns.ReleaseAndGetAddressOf())
	);

	// 頂点シェーダの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/SimpleShape2DInstansing_VS.cso");

	DX::ThrowIfFailed(
		device->CreateVertexShader(vs.data(), vs.size(), nullptr, m_vsIns.ReleaseAndGetAddressOf())
	);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/SimpleRectangleInstansing_PS.cso");

	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_psIns.ReleaseAndGetAddressOf())
	);

	// インプットレイアウトの作成
	device->CreateInputLayout(
		&INPUT_LAYOUT_INSTANCING[0],
		static_cast<UINT>(INPUT_LAYOUT_INSTANCING.size()),
		vs.data(), vs.size(),
		m_inputLayoutIns.ReleaseAndGetAddressOf());
}

void Minimap::DrawInstancing()
{
	D3D11_MAPPED_SUBRESOURCE msr;

	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	// **** バッファの設定 ****

	// 頂点バッファの設定
	{
		context->Map(m_vertexBufferIns.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		auto* vertexData = static_cast<VertexData*>(msr.pData);
		// 頂点データを「Z」の字の順に並べ替えます 三角形2つ分で四角形を表現
		vertexData[0].position = SimpleMath::Vector3(-0.5f, 0.5f, 0.0f); // 左上 (Top-Left)
		vertexData[1].position = SimpleMath::Vector3(0.5f, 0.5f, 0.0f); // 右上 (Top-Right)
		vertexData[2].position = SimpleMath::Vector3(-0.5f, -0.5f, 0.0f); // 左下 (Bottom-Left)
		vertexData[3].position = SimpleMath::Vector3(0.5f, -0.5f, 0.0f); // 右下 (Bottom-Right)

		// UVも位置に対応させる
		vertexData[0].texCoord = SimpleMath::Vector2(0.0f, 0.0f); // 左上
		vertexData[1].texCoord = SimpleMath::Vector2(1.0f, 0.0f); // 右上
		vertexData[2].texCoord = SimpleMath::Vector2(0.0f, 1.0f); // 左下
		vertexData[3].texCoord = SimpleMath::Vector2(1.0f, 1.0f); // 右下

		for (int i = 0; i < 4; i++)
		{
			vertexData[i].color = SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		// バッファのアンマップ
		context->Unmap(m_vertexBufferIns.Get(), 0);
	}

	auto outputSize = m_pCommonResources->GetDeviceResources()->GetOutputSize();
	auto mapSize =  SimpleMath::Vector2(outputSize.right, outputSize.bottom);
	int instanceCount = 0;
	// インスタンスバッファの設定
	{
		context->Map(m_instanceBufferIns.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		auto* instanceData = static_cast<InstanceBuffer*>(msr.pData);
		
		// インスタンスデータの設定
		SetInstanceBufferGameObject(instanceData, &instanceCount, mapSize, GameObjectTag::BUILDING, BUILDING_MARK_COLOR);
		SetInstanceBufferGameObject(instanceData, &instanceCount, mapSize, GameObjectTag::PLAYER, PLAYER_MARK_COLOR);
		SetInstanceBufferGameObject(instanceData, &instanceCount, mapSize, GameObjectTag::ENEMY, ENEMY_MARK_COLOR);
		SetInstanceBufferGameObject(instanceData, &instanceCount, mapSize, GameObjectTag::TREASURE, TREASURE_MARK_COLOR);
		SetInstanceBufferGameObject(instanceData, &instanceCount, mapSize, GameObjectTag::CHECKPOINT, CHECKPOINT_MARK_COLOR);
		SetInstanceBufferGameObject(instanceData, &instanceCount, mapSize, GameObjectTag::ESCAPE_HELICOPTER, HELICOPTER_MARK_COLOR);
		// バッファのアンマップ
		context->Unmap(m_instanceBufferIns.Get(), 0);
	}

	// 定数バッファの設定
	{
		context->Map(m_constantBufferIns.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		auto* constantData = static_cast<ConstantBuffer*>(msr.pData);
		constantData->windowSize = mapSize;
		constantData->scale =SimpleMath::Vector2( 1.0f, 1.0f);
		constantData->dummy = SimpleMath::Vector2(0.0f, 0.0f);
		constantData->position = SimpleMath::Vector2(0.0f, 0.0f);
		// バッファのアンマップ
		context->Unmap(m_constantBufferIns.Get(), 0);
	}

	// **** それぞれのバッファをセットする ****

	// 頂点バッファ
	ID3D11Buffer* pBuf[2]{ m_vertexBufferIns.Get(), m_instanceBufferIns.Get() };
	UINT stride[2]{ sizeof(VertexData), sizeof(InstanceBuffer) };
	UINT offset[2]{ 0, 0 };
	context->IASetVertexBuffers(0, 2, pBuf, stride, offset);
	// インデックスバッファ
	context->IASetIndexBuffer(m_indexBufferIns.Get(), DXGI_FORMAT_R16_UINT, 0);

	// 定数バッファ
	ID3D11Buffer* cbuff[] = { m_constantBufferIns.Get() };
	context->VSSetConstantBuffers(0, 1, cbuff);
	//context->GSSetConstantBuffers(0, 1, cbuff);
	context->PSSetConstantBuffers(0, 1, cbuff);

	// シェーダの設定
	context->VSSetShader(m_vsIns.Get(), nullptr, 0);
	//context->GSSetShader(m_gsIns.Get(), nullptr, 0);
	context->PSSetShader(m_psIns.Get(), nullptr, 0);
	// インプットレイアウトの設定
	context->IASetInputLayout(m_inputLayoutIns.Get());

	// プリミティブトポロジーの設定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->OMSetDepthStencilState(m_pCommonResources->GetCommonStates()->DepthDefault(), 0);
	//	カリングはなし
	context->RSSetState(m_pCommonResources->GetCommonStates()->CullNone());

	// インスタンシング描画の実行
	context->DrawIndexedInstanced(
		6,											//	1インスタンスあたりのインデックス数
		static_cast<UINT>(instanceCount),	//	インスタンス数
		0,											//	インデックスバッファの先頭からのオフセット
		0,											//	頂点バッファの先頭からのオフセット
		0											//	インスタンスバッファの先頭からのオフセット
	);

	// **** バッファの解除 ****
	ID3D11Buffer* nullBuff[2] = { nullptr, nullptr };
	context->IASetVertexBuffers(0, 2, nullBuff, stride, offset);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);

	// 定数バッファの解除
	ID3D11Buffer* nullCBuff[1] = { nullptr };
	context->VSSetConstantBuffers(0, 1, nullCBuff);
	context->GSSetConstantBuffers(0, 1, nullCBuff);
	context->PSSetConstantBuffers(0, 1, nullCBuff);

	// シェーダの解除
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}

/**
 * @brief オフスクリーンレンダリングを使用してミニマップテクスチャの作成
  */
void Minimap::CreateMinimapTexture()
{
	using namespace SimpleMath;
	// 処理用変数
	auto deviceResources = m_pCommonResources->GetDeviceResources();
	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_pCommonResources->GetCommonStates();


	RECT windowRect = deviceResources->GetOutputSize();

	DrawInstancing();
	// オフスクリーンレンダリングの開始
	m_offscreenRendering->Begin(Color(MINIMAP_BACK_COLOR));
	{
		
		//// **** ビューポートの変更 ****
		//D3D11_VIEWPORT vp = {};
		//vp.Width	= m_mapSize.x;	//ビューポートの横幅
		//vp.Height	= m_mapSize.y;	//ビューポートの横幅
		//vp.TopLeftX = 0.0f;			//ビューポートの左上のX座標(ピクセル数）
		//vp.TopLeftY = 0.0f;			//ビューポートの左上のY座標(ピクセル数）
		//vp.MinDepth = 0.0f;			//ビューポートの最小深度（0～1)
		//vp.MaxDepth = 1.0f;			//ビューポートの最大深度（0～1)
		//context->RSSetViewports(1, &vp);



		////	半透明描画指定
		//ID3D11BlendState* blendstate = states->NonPremultiplied();
		////	透明判定処理
		//context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
		////	深度バッファに書き込み参照する
		//context->OMSetDepthStencilState(states->DepthDefault(), 0);
		//// カリング
		//context->RSSetState(states->CullCounterClockwise());
		//context->IASetInputLayout(m_inputLayout.Get());


		//// 頂点の設定
		//DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM] =
		//{
		//	{Vector3(0.0f, 0.0f, 0.0f), Vector4::One, Vector2(0.0f, 0.0f)},
		//};

		//// シェーダの設定
		//context->PSSetShader(m_ps_Circle.Get(), nullptr, 0);
		//context->GSSetShader(m_gs.Get(), nullptr, 0);
		//context->VSSetShader(m_vs.Get(), nullptr, 0);

		//// 定数バッファにデータを設定する
		//ConstantBuffer cb = {};
		//// 表示座標の設定
		//cb.position = Vector2(XMConvertToRadians( 30.0f), 0.0f);
		//cb.windowSize = m_mapSize;
		////cb.windowSize = Vector2(windowRect.right / 2.0f, windowRect.bottom / 2.0f);
		//cb.scale = Vector2(MARK_SIZE, MARK_SIZE) * Screen::Get()->GetScreenScale();
		//cb.dummy = Vector2(static_cast<float>(windowRect.right), static_cast<float>(windowRect.bottom));
		//

		////	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
		//context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &cb, 0, 0);
		//ID3D11Buffer* cbuff[] = { m_constantBuffer.Get() };

		//context->PSSetConstantBuffers(0, 1, cbuff);
		//context->GSSetConstantBuffers(0, 1, cbuff);
		//context->VSSetConstantBuffers(0, 1, cbuff);

		//context->PSSetShader(m_ps_Rectangle.Get(), nullptr, 0);

		//// 各描画処理
		//{
		//	m_primitiveBatch->Begin();
		//	// ビルの描画
		//	DrawBuilding(vertexes, m_mapSize);
		//	m_primitiveBatch->End();
		//}

		//context->PSSetShader(m_ps_Circle.Get(), nullptr, 0);
		//{
		//	m_primitiveBatch->Begin();

		//	// 敵の描画
		//	DrawEnemy(vertexes, m_mapSize);
		//	// お宝の描画
		//	DrawTreasure(vertexes, m_mapSize);
		//	// ヘリコプターの描画
		//	DrawHelicopter(vertexes, m_mapSize);
		//	// チェックポイントの描画
		//	DrawCheckpoint(vertexes, m_mapSize);
		//	m_primitiveBatch->End();
		//}

		//{

		//	// プレイヤーの描画
		//	DrawPlayer(vertexes, m_mapSize);
		//}
		////	シェーダの登録を解除しておく
		//context->VSSetShader(nullptr, nullptr, 0);
		//context->GSSetShader(nullptr, nullptr, 0);
		//context->PSSetShader(nullptr, nullptr, 0);
	}
	// オフスクリーンレンダリングの終了
	m_offscreenRendering->End();

	// **** ビューポートを元に戻す ****
	//auto const viewport = deviceResources->GetScreenViewport();
	//context->RSSetViewports(1, &viewport);
}

void Minimap::DrawPlayer(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;

	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	context->PSSetShader(m_ps_Triangle.Get(), nullptr, 0);


	// プレイヤー座標を取得する
	const auto playerPosition = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER)->GetTransform()->GetPosition();

	// ミニマップ上の座標の算出
	Vector2 minimapPosition = CalcMinimapPosition(playerPosition, m_mapSize);

	// 頂点情報の登録
	for (int i = 0; i < VERTEX_NUM; i++)
	{
		vertexes[i].color = PLAYER_MARK_COLOR;

		// 図形の描画座標(2D)として扱われる
		vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
	}
	m_primitiveBatch->Begin();

	// 描画
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	m_primitiveBatch->End();


}

void Minimap::DrawBuilding(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;

	// 建物を取得する
	const auto buildings = GameObjectRegistry::GetInstance()->GetGameObjects(GameObjectTag::BUILDING);

	for (auto& building : buildings)
	{
		if (!building->IsActive()) { continue; }
		// 座標の取得
		Vector3 buildingPosition = building->GetTransform()->GetPosition();
		// ミニマップ上の座標の算出
		Vector2 minimapPosition = CalcMinimapPosition(buildingPosition, m_mapSize);

		// 頂点情報の登録
		for (int i = 0; i < VERTEX_NUM; i++)
		{
			vertexes[i].color = BUILDING_MARK_COLOR;

			// 図形の描画座標(2D)として扱われる
			vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
		}

		// 描画
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	}
}

void Minimap::DrawTreasure(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;

	// お宝を取得する
	const auto treasures = GameObjectRegistry::GetInstance()->GetGameObjects(GameObjectTag::TREASURE);

	for (auto& treasure : treasures)
	{
		if (!treasure->IsActive()) { continue; }
		// 座標の取得
		Vector3 treasurePosition = treasure->GetTransform()->GetPosition();
		// ミニマップ上の座標の算出
		Vector2 minimapPosition = CalcMinimapPosition(treasurePosition, m_mapSize);

		// 頂点情報の登録
		for (int i = 0; i < VERTEX_NUM; i++)
		{
			vertexes[i].color = TREASURE_MARK_COLOR;

			// 図形の描画座標(2D)として扱われる
			vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
		}

		// 描画
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	}
}

void Minimap::DrawCheckpoint(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;

	// チェックポイントを取得する
	const auto checkpoints = GameObjectRegistry::GetInstance()->GetGameObjects(GameObjectTag::CHECKPOINT);

	for (auto& checkpoint : checkpoints)
	{
		if (!checkpoint->IsActive()) { continue; }
		// 座標の取得
		Vector3 checkpointPosition = checkpoint->GetTransform()->GetPosition();
		// ミニマップ上の座標の算出
		Vector2 minimapPosition = CalcMinimapPosition(checkpointPosition, m_mapSize);

		// 頂点情報の登録
		for (int i = 0; i < VERTEX_NUM; i++)
		{
			vertexes[i].color = CHECKPOINT_MARK_COLOR;

			// 図形の描画座標(2D)として扱われる
			vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
		}

		// 描画
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	}
}

void Minimap::DrawEnemy(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;
	// 敵を取得する
	const auto enemies = GameObjectRegistry::GetInstance()->GetGameObjects(GameObjectTag::ENEMY);

	for (auto& enemy : enemies)
	{
		if (!enemy->IsActive()) { continue; }
		// 座標の取得
		Vector3 enemyPosition = enemy->GetTransform()->GetPosition();
		// ミニマップ上の座標の算出
		Vector2 minimapPosition = CalcMinimapPosition(enemyPosition, m_mapSize);

		// 頂点情報の登録
		for (int i = 0; i < VERTEX_NUM; i++)
		{
			vertexes[i].color = ENEMY_MARK_COLOR;

			// 図形の描画座標(2D)として扱われる
			vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
		}

		// 描画
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	}
}

void Minimap::DrawHelicopter(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;

	// ヘリコプターを取得する
	const auto helicopters = GameObjectRegistry::GetInstance()->GetGameObjects(GameObjectTag::ESCAPE_HELICOPTER);

	for (auto& helicopter : helicopters)
	{
		if (!helicopter->IsActive()) { continue; }
		// 座標の取得
		Vector3 helicopterPosition = helicopter->GetTransform()->GetPosition();
		// ミニマップ上の座標の算出
		Vector2 minimapPosition = CalcMinimapPosition(helicopterPosition, m_mapSize);

		// 頂点情報の登録
		for (int i = 0; i < VERTEX_NUM; i++)
		{
			vertexes[i].color = HELICOPTER_MARK_COLOR;

			// 図形の描画座標(2D)として扱われる
			vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
		}

		// 描画
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	}
}


/**
 * @brief ミニマップ上の座標の算出
 *
 * @param[in] worldPosition ワールド座標
 * @param[in] m_mapSize		マップサイズ
 *
 * @return ミニマップ状の座標
 */
DirectX::SimpleMath::Vector2 Minimap::CalcMinimapPosition(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Vector2& m_mapSize)
{
	using namespace SimpleMath;

	// マップの中心座標
	Vector2 mapCenterPosition(0.0f, 0.0f);

	// 中心からのベクトル
	Vector2 centerToVector = Vector2(worldPosition.x, worldPosition.z) - mapCenterPosition;


	// 距離をミニマップ表示のサイズ感へ縮める
	Vector2 normalDirection = centerToVector / (SPACE_VALUE / Screen::Get()->GetScreenScale());

	// ミニマップ地点
	Vector2 windowCenter = Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF());
	Vector2 windowLeftTop = Vector2(0.f, 0.f);
	Vector2 minimapPosition = normalDirection + windowLeftTop;

	RECT windowRect = m_pCommonResources->GetDeviceResources()->GetOutputSize();
	minimapPosition.x -= (windowRect.right * 0.5f - m_mapSize.x * 0.5f);
	minimapPosition.y -= (windowRect.bottom * 0.5f - m_mapSize.y * 0.5f);

	return minimapPosition;
}
//
//void Minimap::SetInstanceBufferBuilding(InstanceBuffer* instanceData, int* currentIndex, const DirectX::SimpleMath::Vector2& mapSize)
//{
//	// 建物を取得する
//	const auto buildings = GameObjectRegistry::GetInstance()->GetGameObjects(GameObjectTag::BUILDING);
//	for (auto& building : buildings)
//	{
//		if (!building->IsActive()) { continue; }
//		
//		// 設定
//		instanceData[*currentIndex] = GetInstanceData(building->GetTransform()->GetPosition(), Minimap::MARK_SIZE, BUILDING_MARK_COLOR, mapSize) ;
//		(*currentIndex)++;
//	}
//}

void Minimap::SetInstanceBufferGameObject(InstanceBuffer* instanceData, int* currentIndex, const DirectX::SimpleMath::Vector2& mapSize, const GameObjectTag& tag, const DirectX::SimpleMath::Vector4& color)
{
	// オブジェクトを取得する
	const auto gameObjects = GameObjectRegistry::GetInstance()->GetGameObjects(tag);
	for (auto& gameObject : gameObjects)
	{
		if (!gameObject->IsActive()) { continue; }
		// 設定
		instanceData[*currentIndex] = GetInstanceData(gameObject->GetTransform()->GetPosition(), Minimap::MARK_SIZE, color, mapSize);
		(*currentIndex)++;
	}
}


Minimap::InstanceBuffer Minimap::GetInstanceData(const DirectX::SimpleMath::Vector3& worldPosition, const float& scale, const DirectX::SimpleMath::Vector4& color, const DirectX::SimpleMath::Vector2& mapSize)
{
	InstanceBuffer instanceData;

	// ワールド行列の設定
	auto translationMat = SimpleMath::Matrix::CreateTranslation(CalcVertexMapPosition(worldPosition, mapSize));
	auto scaleMat = SimpleMath::Matrix::CreateScale(MARK_SIZE * Screen::Get()->GetScreenScale(), MARK_SIZE * Screen::Get()->GetScreenScale(), 1.0f);
	auto worldMatrix = scaleMat * translationMat;

	instanceData.world = worldMatrix;
	// 色の設定
	instanceData.color = color;

	return instanceData;
}

DirectX::SimpleMath::Vector3 Minimap::CalcVertexMapPosition(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Vector2& mapSize)
{
	// ワールド行列の設定
	auto minimapPos = CalcMinimapPosition(worldPosition, mapSize);
	auto convertedPos = SimpleMath::Vector3(minimapPos.x / (mapSize.x / 2.0f), -(minimapPos.y / (mapSize.y / 2.0f)), 0.0f);

	return convertedPos;
}
