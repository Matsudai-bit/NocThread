/*****************************************************************//**
 * @file    CircularShadow.h
 * @brief   丸影に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/08
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CircularShadow.h"

#include "Game/Common/Graphics/Camera/Camera.h"
#include "Library/DirectXFramework/ReadData.h"

using namespace DirectX;

///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> CircularShadow::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },

};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CircularShadow::CircularShadow()
	: m_castPositionY {}
	, m_pDeviceResources{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
CircularShadow::~CircularShadow()
{

}


/**
 * @brief 初期化処理
 * 
 * @param[in] pDeviceResources		デバイスリソース
 * @param[in] castShadowPositionY　	影を落とすY座標
 */
void CircularShadow::Initialize(DX::DeviceResources* pDeviceResources, const float& castShadowPositionY)
{
	m_pDeviceResources = pDeviceResources;

	auto context = m_pDeviceResources->GetD3DDeviceContext();
	auto device = m_pDeviceResources->GetD3DDevice();

	// ベーシックエフェクトの作成
	//m_basicEffect = std::make_unique<BasicEffect>(device);

	//// 設定
	//m_basicEffect->SetTextureEnabled(false);
	//m_basicEffect->SetVertexColorEnabled(true);
	//m_basicEffect->SetLightEnabled(0, false);
	//m_basicEffect->SetLightEnabled(1, false);
	//m_basicEffect->SetLightEnabled(2, false);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context);

	//// インプットレイアウトの作成
	//CreateInputLayoutFromEffect<VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());



	m_castPositionY = castShadowPositionY;

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/CircleShadow_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
	);

	// ジオメトリシェーダの作成
	std::vector<uint8_t> gs = DX::ReadData(L"Resources/Shaders/CircleShadow_GS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(gs.data(), gs.size(), nullptr, m_gs.ReleaseAndGetAddressOf())
	);

	// バーテックスの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/CircleShadow_VS.cso");
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
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 * @param[in] states 共通リソース
 * @param[in] shadowOwnerPosition　影の所有者座標
 */
void CircularShadow::Draw(const Camera& camera, DirectX::CommonStates* states, const DirectX::SimpleMath::Vector3& shadowOwnerPosition)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();


	// 頂点の設定
	DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM] =
	{
		{SimpleMath::Vector3(0.0f,  0.0f, 0.0f),SimpleMath::Vector4::One, SimpleMath::Vector2(0.5f, 0.5f)},
	};
	auto position = SimpleMath::Vector3(shadowOwnerPosition.x, m_castPositionY, shadowOwnerPosition.z);

	// 地面との距離を計算
	float distance =  std::abs(shadowOwnerPosition.y - m_castPositionY);

	float scale = std::max(1.5f - distance * 0.05f, 0.0f);

	// ワールド座標の算出
	auto world = SimpleMath::Matrix::CreateScale(scale) * SimpleMath::Matrix::CreateTranslation(position) ;


	// 定数バッファにデータを設定する
	ConstantBuffer cb = {};
	cb.matWorldTranspose = world.Transpose();
	cb.matProj = camera.GetProjectionMatrix().Transpose();
	cb.matView = camera.GetViewMatrix().Transpose();
	cb.alphaValue = SimpleMath::Vector4(1.0f / (distance * 0.4f));

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
