/*****************************************************************//**
 * @file    ConcentrationLines.h
 * @brief   シンプルなパーティクルエフェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/12
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ConcentrationLines.h"

#include "Game/Common/Shadow/CircularShadow/CircularShadow.h"
#include "Library/DirectXFramework/ReadData.h"
#include "Game/Common/Camera/Camera.h"
using namespace DirectX;


///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> ConcentrationLines::INPUT_LAYOUT =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                             D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ConcentrationLines::ConcentrationLines(DX::DeviceResources* pDeviceResources, const float& speed, const float& lineLengthRate)
	: m_commonStates{pDeviceResources->GetD3DDevice()}
	, m_pDeviceResources{ pDeviceResources }
	, m_elapsedTimeCounter{}
	, m_speed{ speed }
	, m_lineLengthRate{ lineLengthRate }
{
	auto device = pDeviceResources->GetD3DDevice();
	auto context = pDeviceResources->GetD3DDeviceContext();

	// **** 描画関連の設定 **** 

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context);
	
	// ピクセルシェーダの作成
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/ConcentrationLines_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
	);

	// ジオメトリシェーダの作成
	std::vector<uint8_t> gs = DX::ReadData(L"Resources/Shaders/ConcentrationLines_GS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(gs.data(), gs.size(), nullptr, m_gs.ReleaseAndGetAddressOf())
	);

	// バーテックスの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/ConcentrationLines_VS.cso");
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
ConcentrationLines::~ConcentrationLines()
{

}




/**
 * @brief 再生
 * 
 */
void ConcentrationLines::Play()
{
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void ConcentrationLines::Update(float deltaTime, const float& isEnd)
{
	if (isEnd)
	{
		// エフェクトの終了を通知
		SetIsPlaying(false);
	}
	
	m_elapsedTimeCounter.UpperTime(deltaTime);



	
}

/**
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 */
void ConcentrationLines::Draw(const Camera& camera)
{
	using namespace SimpleMath;

	UNREFERENCED_PARAMETER(camera);

	auto context = m_pDeviceResources->GetD3DDeviceContext();
	
	
	// ワールド行列の算出
	auto world = Matrix::Identity;


	// 定数バッファにデータを設定する
	ConstantBuffer cb = {};
	cb.lineColor = Vector3(1.0f, 1.0f, 1.0f);
	cb.time = static_cast<float>(m_elapsedTimeCounter.GetElapsedTime());
	cb.speed = m_speed;
	cb.noiseScale = 80.0f;
	cb.uvOffset = Vector2(0.5f, 0.5f);
	cb.lineRegion = 0.25f;
	cb.lineRate = m_lineLengthRate;

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
	ID3D11BlendState* blendState = m_commonStates.NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_commonStates.DepthRead(), 0);

	//	カリングはなし
	context->RSSetState(m_commonStates.CullCounterClockwise());

	//m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	//	頂点情報(板ポリゴンの４頂点の座標情報）
	VertexPositionColorTexture vertex[4] =
	{
		VertexPositionColorTexture(SimpleMath::Vector3(0.0f,  0.0f, 0.0f),SimpleMath::Vector4::One,SimpleMath::Vector2(0.0f, 0.0f)),
	};

	m_primitiveBatch->Begin();

	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 1);
	
	m_primitiveBatch->End();

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}

