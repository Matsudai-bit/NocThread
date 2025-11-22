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
#include "Game/Common/DeviceResources.h"

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
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
	);

	// ジオメトリシェーダの作成
	std::vector<uint8_t> gs = DX::ReadData(L"Resources/Shaders/SimpleCircle_GS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(gs.data(), gs.size(), nullptr, m_gs.ReleaseAndGetAddressOf())
	);

	// バーテックスの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/SimpleCircle_VS.cso");
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
	// 処理用変数
	auto deviceResources	= m_pCommonResources->GetDeviceResources();
	auto context			= m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states				= m_pCommonResources->GetCommonStates();

	// 色の設定
	SimpleMath::Vector4 color(1.0f, 0.0f, 0.0f, 1.0f);

	// 頂点の設定
	DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM] =
	{
		{SimpleMath::Vector3(0.0f, 0.0f, 0.0f), color, SimpleMath::Vector2(0.0f, 0.0f)},
	};

	// **** テスト ****
	SimpleMath::Vector2 mapCenter;

	SimpleMath::Vector3 playerPosition = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::PLAYER)->GetTransform()->GetPosition();

	SimpleMath::Vector2 toPlayerDirection = playerPosition - mapCenter;
	toPlayerDirection.Normalize();
	float lengthSquare = SimpleMath::Vector2::DistanceSquared(mapCenter, SimpleMath::Vector2(playerPosition.x, playerPosition.z));

	// 距離をミニマップ表示のサイズ感へ縮める
	float normalLength = lengthSquare / 100.0f;

	// ミニマップ地点
	SimpleMath::Vector2 windowCenter = SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF());
	SimpleMath::Vector2 minimapPosition = (normalLength * toPlayerDirection) + windowCenter;

	// 定数バッファにデータを設定する
	ConstantBuffer cb = {};
	// 表示座標の設定
	cb.position	= minimapPosition;
	cb.windowSize	= SimpleMath::Vector2(deviceResources->GetOutputSize().right, deviceResources->GetOutputSize().bottom);
	cb.scale		= SimpleMath::Vector2(0.25f, 0.25f);

	cb.dummy	= SimpleMath::Vector2(deviceResources->GetOutputSize().right, deviceResources->GetOutputSize().bottom);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &cb, 0, 0);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	ID3D11Buffer* cbuff[] = { m_constantBuffer.Get() };
	context->PSSetConstantBuffers(0, 1, cbuff);
	context->GSSetConstantBuffers(0, 1, cbuff);
	context->VSSetConstantBuffers(0, 1, cbuff);

	// シェーダの設定
	context->PSSetShader(m_ps.Get(), nullptr, 0);
	context->GSSetShader(m_gs.Get(), nullptr, 0);
	context->VSSetShader(m_vs.Get(), nullptr, 0);

	//	半透明描画指定
	ID3D11BlendState* blendstate = states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// カリング
	context->RSSetState(states->CullCounterClockwise());

	//m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], 1);
	m_primitiveBatch->End();
	//m_primitiveBatch->Begin();
	//m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, VERTEX_NUM + 2, vertexes, static_cast<size_t>(VERTEX_NUM));
	//m_primitiveBatch->End();

		//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
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
