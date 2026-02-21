/*****************************************************************//**
 * @file    ColliderDrawer.h
 * @brief   XXXXに関するソースファイル
 *
 * @author  松下大暉
 * @date    ××××/××/××
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ColliderDrawer.h"



// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連

// ゲームプレイロジック関連

// グラフィック関連

using namespace DirectX;
// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ColliderDrawer::ColliderDrawer(DX::DeviceResources* pDeviceResources)
	:  m_primitiveBatch{ pDeviceResources->GetD3DDeviceContext() }
	, m_inputLayout{}
	, m_basicEffect{ pDeviceResources->GetD3DDevice() }
	, m_pDeviceResources{ pDeviceResources } {
	// ライト(OFF)
	m_basicEffect.SetLightingEnabled(false);
	// 頂点カラー(ON)
	m_basicEffect.SetVertexColorEnabled(true);
	// テクスチャ(OFF)
	m_basicEffect.SetTextureEnabled(false);

	// 入力レイアウト作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			pDeviceResources->GetD3DDevice(), &m_basicEffect, m_inputLayout.ReleaseAndGetAddressOf())
	);

}



/**
 * @brief デストラクタ
 */
ColliderDrawer::~ColliderDrawer()
{

}

void ColliderDrawer::DrawTriangle(const Triangle& triangle, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	static constexpr DirectX::XMFLOAT4 COLOR = DirectX::XMFLOAT4(255.0f, 0.0f, 0.0f, 255.0f);

	auto context = m_pDeviceResources->GetD3DDeviceContext();

	DirectX::VertexPositionColor vertexes[VERTEX_NUM]{};		// 頂点データ

	for (size_t i = 0; i < VERTEX_NUM; i++)
	{
		vertexes[i].position = triangle.GetPosition()[i];
		vertexes[i].color = COLOR;

	}


	// 入力レイアウト
	context->IASetInputLayout(m_inputLayout.Get());

	m_basicEffect.SetWorld(SimpleMath::Matrix::Identity);

	m_basicEffect.SetView(view);
	m_basicEffect.SetProjection(projection);

	// 適用
	m_basicEffect.Apply(context);

	// プリミティブバッチの描画
	m_primitiveBatch.Begin();

	// 三角形の描画
	m_primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, INDEXES, 3, vertexes, 3);

	m_primitiveBatch.End();
}

void ColliderDrawer::DrawBox2D(const Box2D& box2D, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{


	for (auto& triangle : box2D.GetTriangle())
	{
		DrawTriangle(triangle, view, projection);
	}
}

void ColliderDrawer::DrawAABB(const AABB& aabb, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	std::unique_ptr<GeometricPrimitive> box = GeometricPrimitive::CreateBox(context, aabb.GetExtend());

	box->Draw(SimpleMath::Matrix::CreateTranslation(aabb.GetCenter()), view, projection, Colors::Red, nullptr, true);
}

void ColliderDrawer::DrawSphere(const Sphere& sphere, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	std::unique_ptr<GeometricPrimitive>  a = GeometricPrimitive::CreateSphere(context, sphere.GetRadius() * 2.0f);
	a->Draw(SimpleMath::Matrix::CreateTranslation(sphere.GetPosition()), view, projection, Colors::White, nullptr, true);
}


