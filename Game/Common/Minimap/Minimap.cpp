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

#include "Game/GameObjects/Common/GameObject.h"
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

	SimpleMath::Vector2 mapSize = RENDERING_SIZE * Screen::Get()->GetScreenScale();
	RECT rect{
		0,
		0,
		static_cast<LONG>(mapSize.x),
		static_cast<LONG>(mapSize.y)

	};

	// オフスクリーンレンダリングの作成
	m_offscreenRendering = std::make_unique<MyLib::OffscreenRendering>(
		pCommonResources->GetDeviceResources(),
		pCommonResources->GetDeviceResources()->GetBackBufferFormat(),
		pCommonResources->GetDeviceResources()->GetDepthBufferFormat());
	m_offscreenRendering->SetDevice(device);
	m_offscreenRendering->SetWindow(rect);


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

	// ミニマップテクスチャの作成
	CreateMinimapTexture();

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
	// 処理用変数
	auto states				= m_pCommonResources->GetCommonStates();

	SimpleMath::Vector2 mapSize = RENDERING_SIZE * Screen::Get()->GetScreenScale();


	RECT rect{
		0,
		0,
		static_cast<LONG>(mapSize.x),
		static_cast<LONG>(mapSize.y)

	};

	// ミニマップの描画
	m_spriteBatch->Begin(SpriteSortMode_Immediate, states->NonPremultiplied());
	m_spriteBatch->Draw(
		m_offscreenRendering->GetShaderResourceView(),
		SimpleMath::Vector2::Zero,
		&rect,
		Colors::White,
		0.0f,SimpleMath::Vector4::Zero,
		0.2f * Screen::Get()->GetScreenScale());

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

	SimpleMath::Vector2 mapSize = RENDERING_SIZE * Screen::Get()->GetScreenScale();


	// オフスクリーンレンダリングの開始
	m_offscreenRendering->Begin(Color(0.01f, 0.01f, 0.48f));
	{

		// **** ビューポートの変更 ****
		D3D11_VIEWPORT vp = {};
		vp.Width = mapSize.x;	//ビューポートの横幅
		vp.Height = mapSize.y;	//ビューポートの横幅
		vp.TopLeftX = 0.0f;			//ビューポートの左上のX座標(ピクセル数）
		vp.TopLeftY = 0.0f;			//ビューポートの左上のY座標(ピクセル数）
		vp.MinDepth = 0.0f;			//ビューポートの最小深度（0～1)
		vp.MaxDepth = 1.0f;			//ビューポートの最大深度（0～1)
		context->RSSetViewports(1, &vp);

		//	半透明描画指定
		ID3D11BlendState* blendstate = states->NonPremultiplied();
		//	透明判定処理
		context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
		//	深度バッファに書き込み参照する
		context->OMSetDepthStencilState(states->DepthDefault(), 0);
		// カリング
		context->RSSetState(states->CullCounterClockwise());
		context->IASetInputLayout(m_inputLayout.Get());


		// 頂点の設定
		DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM] =
		{
			{Vector3(0.0f, 0.0f, 0.0f), Vector4::One, Vector2(0.0f, 0.0f)},
		};

		// シェーダの設定
		context->PSSetShader(m_ps_Circle.Get(), nullptr, 0);
		context->GSSetShader(m_gs.Get(), nullptr, 0);
		context->VSSetShader(m_vs.Get(), nullptr, 0);

		// 定数バッファにデータを設定する
		ConstantBuffer cb = {};
		// 表示座標の設定
		cb.position = Vector2(static_cast<float>(windowRect.right), static_cast<float>(windowRect.bottom));
		cb.windowSize = mapSize;
		//cb.windowSize = Vector2(windowRect.right / 2.0f, windowRect.bottom / 2.0f);
		cb.scale = Vector2(MARK_SIZE, MARK_SIZE) * Screen::Get()->GetScreenScale();
		cb.dummy = Vector2(static_cast<float>(windowRect.right), static_cast<float>(windowRect.bottom));

		//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
		context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &cb, 0, 0);
		ID3D11Buffer* cbuff[] = { m_constantBuffer.Get() };

		context->PSSetConstantBuffers(0, 1, cbuff);
		context->GSSetConstantBuffers(0, 1, cbuff);
		context->VSSetConstantBuffers(0, 1, cbuff);

		context->PSSetShader(m_ps_Rectangle.Get(), nullptr, 0);

		// 各描画処理
		m_primitiveBatch->Begin();
		// ビルの描画
		DrawBuilding(vertexes, mapSize);
		m_primitiveBatch->End();

		context->PSSetShader(m_ps_Circle.Get(), nullptr, 0);

		m_primitiveBatch->Begin();

		// 敵の描画
		DrawEnemy(vertexes, mapSize);
		// お宝の描画
		DrawTreasure(vertexes, mapSize);
		// ヘリコプターの描画
		DrawHelicopter(vertexes, mapSize);
		// プレイヤーの描画
		DrawPlayer(vertexes, mapSize);
		m_primitiveBatch->End();

		//	シェーダの登録を解除しておく
		context->VSSetShader(nullptr, nullptr, 0);
		context->GSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
	}
	// オフスクリーンレンダリングの終了
	m_offscreenRendering->End();

	// **** ビューポートを元に戻す ****
	auto const viewport = deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
}

void Minimap::DrawPlayer(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize)
{
	using namespace SimpleMath;

	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	context->PSSetShader(m_ps_Circle.Get(), nullptr, 0);


	// プレイヤー座標を取得する
	const auto playerPosition= GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER)->GetTransform()->GetPosition();

	// ミニマップ上の座標の算出
	Vector2 minimapPosition = CalcMinimapPosition(playerPosition, mapSize);

	// 頂点情報の登録
	for (int i = 0; i < VERTEX_NUM; i++)
	{
		vertexes[i].color = PLAYER_MARK_COLOR;

		// 図形の描画座標(2D)として扱われる
		vertexes[i].position = Vector3(minimapPosition.x, minimapPosition.y, 0.0f);
	}

	// 描画
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);

	
}

void Minimap::DrawBuilding(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize)
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
		Vector2 minimapPosition = CalcMinimapPosition(buildingPosition, mapSize);

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

void Minimap::DrawTreasure(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize)
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
		Vector2 minimapPosition = CalcMinimapPosition(treasurePosition, mapSize);

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

void Minimap::DrawEnemy(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize)
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
		Vector2 minimapPosition = CalcMinimapPosition(enemyPosition, mapSize);

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

void Minimap::DrawHelicopter(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize)
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
		Vector2 minimapPosition = CalcMinimapPosition(helicopterPosition, mapSize);

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
 * @param[in] mapSize		マップサイズ
 * 
 * @return ミニマップ状の座標
 */
DirectX::SimpleMath::Vector2 Minimap::CalcMinimapPosition(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Vector2& mapSize)
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
	Vector2 minimapPosition = normalDirection + windowCenter;

	RECT windowRect = m_pCommonResources->GetDeviceResources()->GetOutputSize();
	minimapPosition.x -= (windowRect.right * 0.5f	- mapSize.x * 0.5f);
	minimapPosition.y -= (windowRect.bottom * 0.5f - mapSize.y * 0.5f);

	return minimapPosition;
}
