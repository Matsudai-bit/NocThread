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
#include "Game/Common/ResourceManager/ResourceManager.h"
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
	m_effect->SetTextureEnabled(false);
	m_effect->SetVertexColorEnabled(true);
	m_effect->SetLightEnabled(0, false);
	m_effect->SetLightEnabled(1, false);
	m_effect->SetLightEnabled(2, false);

	// インプットレイアウトの作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	m_ropeModel = std::make_unique<Model>(pCommonResouces->GetResourceManager()->CreateModel("rope.sdkmesh"));
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

	auto cylinder = GeometricPrimitive::CreateCylinder(context, 1.f, 0.5f, 10, false);
	auto sphere = GeometricPrimitive::CreateSphere(context, 0.5f);

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

	//m_batch->Begin();
	// 線の描画処理
	for (size_t i = 1; i < m_particles.size(); i++)
	{
		// 円同士の中点を計算
		auto startPos = m_particles[i - 1]->GetPosition();
		auto endPos = m_particles[i]->GetPosition();

		auto centerPos = startPos;// (startPos + endPos) / 2.0f;

		// ロープの向き
		auto direction = endPos - startPos;

		if (direction.Length() <= 0.0f)
		{
			continue;
		}

		// ロープの長さ
		float length = direction.Length();

		// ロープの回転
		direction.Normalize();
		auto up = SimpleMath::Vector3::Up;
		auto right = up.Cross(direction);
		right.Normalize();

		if (right.Length() <= 0.0f)
		{
			right = SimpleMath::Vector3::Right;
		}

		SimpleMath::Quaternion q;
		q = SimpleMath::Quaternion::CreateFromAxisAngle(right, acosf(direction.Dot(SimpleMath::Vector3::Up)));

		SimpleMath::Matrix rotation = SimpleMath::Matrix::CreateFromQuaternion(q);
		SimpleMath::Matrix translation = SimpleMath::Matrix::CreateTranslation(centerPos);
		SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(0.02f, length*0.1f , 0.02f);

		// ワールド行列
		world = scale * rotation * translation;
		
		m_ropeModel->Draw(context, *pStates, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());


	}
//	m_batch->End();
	// 深度ステンシルバッファの設定
	context->OMSetDepthStencilState(pStates->DepthDefault(), 0);

	// ブレンドステートの設定
	context->OMSetBlendState(pStates->AlphaBlend(), nullptr, 0xffffffff);

	// カリングの設定
	context->RSSetState(pStates->CullNone());              // カリングしない
	context->RSSetState(pStates->CullClockwise());              // カリングしない


	// パーティクルの移動ベクトル表示（デバッグ用）
	//// ワールド行列
	//m_effect->SetWorld(world);
	//// ビュー行列
	//m_effect->SetView(camera.GetViewMatrix());
	//// 射影行列
	//m_effect->SetProjection(camera.GetProjectionMatrix());

	//m_effect->SetColorAndAlpha(DirectX::Colors::Yellow);
	//// エフェクトを適応する
	//m_effect->Apply(context);

	//// 入力レイアウト
	//context->IASetInputLayout(m_inputLayout.Get());


	//m_batch->Begin();
	//for (size_t i = 0; i < m_particles.size(); i++)
	//{
	//	VertexPositionColor vertex[2]{};
	//	vertex[0].color = SimpleMath::Vector4(1, 1.0f, 1.0f, 1);
	//	vertex[1].color = SimpleMath::Vector4(1, 1.0f, 1.0f, 1);
	//	vertex[0].position = m_particles[i]->GetPosition();
	//	auto direction = m_particles[i]->GetVelocity();
	//	direction.Normalize();
	//	vertex[1].position = m_particles[i]->GetPosition() + direction * 1.5f;
	//	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, vertex, 2);
	//}
	//m_batch->End();



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
