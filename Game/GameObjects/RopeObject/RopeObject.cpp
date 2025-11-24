/*****************************************************************//**
 * @file    RopeObject.h
 * @brief   ロープオブジェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "RopeObject.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/DirectXFramework/DeviceResources.h"
#include "Game/Common/Camera/Camera.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
RopeObject::RopeObject()
	: m_pCommonResources{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
RopeObject::~RopeObject()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void RopeObject::Initialize(const CommonResources* pCommonResouces)
{
	m_pCommonResources = pCommonResouces;
	auto device = pCommonResouces->GetDeviceResources()->GetD3DDevice();
	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	m_particles.clear();


	// プリミティブバッチの作成
	m_batch = std::make_unique <PrimitiveBatch<VertexPositionColor>>(context);

	// エフェクトの作成
	m_effect = std::make_unique<BasicEffect>(device);

	// インプットレイアウトの作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf()
		)
	);
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void RopeObject::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void RopeObject::Draw(const Camera& camera)
{
	auto context = m_pCommonResources->GetDeviceResources()->GetD3DDeviceContext();

	auto pStates = m_pCommonResources->GetCommonStates();



	// 深度ステンシルバッファの設定
	context->OMSetDepthStencilState(pStates->DepthDefault(), 0);

	// ブレンドステートの設定
	context->OMSetBlendState(pStates->AlphaBlend(), nullptr, 0xffffffff);

	// カリングの設定
	context->RSSetState(pStates->CullNone());              // カリングしない
	context->RSSetState(pStates->CullClockwise());              // カリングしない


	// テクスチャサンプラーの設定
	ID3D11SamplerState* samplers[] = { pStates->PointWrap() };
	context->PSSetSamplers(0, 1, samplers);



	// ワールド行列
	SimpleMath::Matrix world = SimpleMath::Matrix::Identity;/* rotationZ* rotationX;*/
	m_effect->SetWorld(world);
	// ビュー行列
	m_effect->SetView(camera.GetViewMatrix());
	// 射影行列
	m_effect->SetProjection(camera.GetProjectionMatrix());

	m_effect->SetColorAndAlpha(DirectX::Colors::Red);
	// エフェクトを適応する
	m_effect->Apply(context);

	// 入力レイアウト
	context->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();
	// 線の描画処理
	for (size_t i = 1; i < m_particles.size(); i++)
	{

		// 頂点
		VertexPositionColor vertex[2]{};
		vertex[0].color = SimpleMath::Vector4(1, 1.0f, 1.0f, 1);
		vertex[1].color = SimpleMath::Vector4(1, 1.0f, 1.0f, 1);

		vertex[0].position = m_particles[i - 1]->GetPosition();
		vertex[1].position = m_particles[i]->GetPosition();

		// 描画
		m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, vertex, 2);
	}
	m_batch->End();



}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void RopeObject::Finalize()
{

}

/**
 * @brief パーティクル情報のリセット
 * 
 */
void RopeObject::ResetParticle()
{
	m_particles.clear();
}

/**
 * @brief パーティクルの追加
 * 
 * @param[in] particle 追加パーティクル
 */
void RopeObject::AddParticle(ParticleObject* pParticle)
{

//	if (std::find(m_particles.begin(), m_particles.back(), particle))
	m_particles.push_back(pParticle);
}

/**
 * @brief パーティクルデータの取得
 * 
 * @return パーティクルデータ
 */
std::vector<ParticleObject*>* RopeObject::GetParticles() 
{
	return &m_particles;
}
