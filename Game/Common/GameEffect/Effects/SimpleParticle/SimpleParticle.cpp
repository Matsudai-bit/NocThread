/*****************************************************************//**
 * @file    SimpleParticle.h
 * @brief   シンプルなパーティクルエフェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/12
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SimpleParticle.h"

#include "Game/Common/Shadow/CircularShadow/CircularShadow.h"
#include "Library/DirectXFramework/ReadData.h"
#include "Game/Common/Camera/Camera.h"
using namespace DirectX;


///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> SimpleParticle::INPUT_LAYOUT =
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
SimpleParticle::SimpleParticle(DX::DeviceResources* pDeviceResources, DirectX::SimpleMath::Vector3 position,const Camera* pCamera)
	: m_commonStates{pDeviceResources->GetD3DDevice()}
	, m_pDeviceResources{ pDeviceResources }
	, m_pCamera{ pCamera }
{
	auto device = pDeviceResources->GetD3DDevice();
	auto context = pDeviceResources->GetD3DDeviceContext();

	// **** 描画関連の設定 **** 

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps = DX::ReadData(L"Resources/Shaders/SimpleParticle_PS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps.data(), ps.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
	);

	// ジオメトリシェーダの作成
	std::vector<uint8_t> gs = DX::ReadData(L"Resources/Shaders/SimpleParticle_GS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(gs.data(), gs.size(), nullptr, m_gs.ReleaseAndGetAddressOf())
	);

	// バーテックスの作成
	std::vector<uint8_t> vs = DX::ReadData(L"Resources/Shaders/SimpleParticle_VS.cso");
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

	// **** 描画関連の設定 ここまで **** 

	m_position = position;

	std::random_device randomDevice;
	m_randomGenerator.seed(randomDevice());

	m_model = GeometricPrimitive::CreateSphere(context, 0.5f);

}



/**
 * @brief デストラクタ
 */
SimpleParticle::~SimpleParticle()
{

}




/**
 * @brief 再生
 * 
 */
void SimpleParticle::Play()
{
}

/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void SimpleParticle::Update(float elapsedTime)
{
	// 時間のカウント
	m_timeCounter.UpperTime(elapsedTime);

	// 時間が来たら
	if (m_timeCounter.GetElapsedTime() >= 0.1f)
	{
		// パーティクルの生成
		CreateParticle();

		// カウンタのリセット
		m_timeCounter.Reset();
	}

	// パーティクルの更新処理
	for (auto& particle : m_particles)
	{
		// 活動時間が終わったら
		if (particle.time <= 0)
		{
			// 活動を終了する
			particle.isActive = false;
			particle.time = 0.0f;
		}
		if (particle.isActive == false){continue;}

		// 座標の更新処理
		particle.position.y += particle.speed * elapsedTime;
		particle.time -= elapsedTime;
	}
}

/**
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 */
void SimpleParticle::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	using namespace SimpleMath;

	auto context = m_pDeviceResources->GetD3DDeviceContext();
	
	// カメラ方向を取得
	Vector3 cameraDirection = m_pCamera->GetTarget()  - m_pCamera->GetEye();
	cameraDirection.Normalize();
	//	パーティクル情報を、カメラからの距離順でソートする
	std::sort(m_particles.begin(), m_particles.end(),
		//	ソート処理の基準を示す関数については、ラムダ式で指定する
		[&](Particle lhs, Particle  rhs)
		{
			//	カメラ正面の距離でソート
			return cameraDirection.Dot(lhs.position - m_pCamera->GetEye()) > cameraDirection.Dot(rhs.position - m_pCamera->GetEye());
		});

	std::vector<VertexPositionColorTexture> vertexes;
	for (auto& particle : m_particles)
	{
		if (!particle.isActive) { continue; }

		VertexPositionColorTexture vPCT;

		// 座標の設定
		vPCT.position = particle.position;

		// 色の設定
		vPCT.color = Vector4(0.6f, 1.0f, 0.1f, 0.0f);

		vertexes.emplace_back(vPCT);
	}
	// ワールド行列の算出
	auto world = Matrix::Identity;


	// 定数バッファにデータを設定する
	ConstantBuffer cb = {};
	cb.matWorldTranspose = world.Transpose();
	cb.matProj = proj.Transpose();
	cb.matView = view.Transpose();
	cb.objectTarget = m_pCamera->GetEye();
	cb.scale = 0.5f;

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
	context->RSSetState(m_commonStates.CullClockwise());

	//m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();

	for (auto& particle : m_particles)
	{
		if (particle.isActive == false) { continue; }

		
		//m_model->Draw(SimpleMath::Matrix::CreateTranslation(particle.position), view, proj,Colors::Green);
		m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertexes[0], static_cast<int>(vertexes.size()));
	}
	m_primitiveBatch->End();

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}

/**
 * @brief プレイ中かどうか
 * 
 * @returns true  プレイ中
 * @returns false プレイしていない
 */
bool SimpleParticle::IsPlaying() const
{
	return true;
}

/**
 * @brief 座標の設定
 * 
 * @param[in] position 座標
 */
void SimpleParticle::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

void SimpleParticle::CreateParticle()
{
	// 生成パーティクル
	Particle* pCreationParticle = nullptr;

	// 活動していないパーティクルを探索
	auto findingParticle = std::find_if(m_particles.begin(), m_particles.end(), 
		[](const Particle& p) {return !(p.isActive); });

	// 見つかっていればそのパーティクルを使用する
	if (findingParticle != m_particles.end())
	{
		pCreationParticle = &(*findingParticle);
	}
	// 見つからなければ新しく生成する
	else
	{
		m_particles.emplace_back();
		pCreationParticle = &m_particles.back();
	}

	
	// 活動時間を決める
	pCreationParticle->time = 1.0f;

	// 速さを決める
	pCreationParticle->speed = 2.0f;

	// 生成位置を決める
	float radius = 2.0f;
	float radian = XMConvertToRadians(static_cast<float>(static_cast<int>(m_randomGenerator()) % 360));
	pCreationParticle->position.y = m_position.y;
	pCreationParticle->position.x = m_position.x + std::cos(radian) * radius;
	pCreationParticle->position.z = m_position.z + std::sin(radian) * radius;


	pCreationParticle->isActive = true;
	

}

