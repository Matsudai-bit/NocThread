/*****************************************************************//**
 * @file    Line2D.h
 * @brief   2Dラインに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/14
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Line2D.h"

#include "Library/DirectXFramework/DeviceResources.h"


#include "Library/DirectXFramework/ReadData.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Line2D::Line2D()
	: m_pDeviceResources{ nullptr }
	, m_length { 1.0f }
	, m_thickness{1.0f}
	, m_radianAngle{}

{

}



/**
 * @brief デストラクタ
 */
Line2D::~Line2D()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Line2D::Initialize(DX::DeviceResources* pDeviceResources)
{
	m_pDeviceResources = pDeviceResources;
    auto device = m_pDeviceResources->GetD3DDevice();
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	//	プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetTextureEnabled(false);
	m_basicEffect->SetVertexColorEnabled(true);
	m_basicEffect->SetLightEnabled(0, false);
	m_basicEffect->SetLightEnabled(1, false);
	m_basicEffect->SetLightEnabled(2, false);

	CreateInputLayoutFromEffect<VertexPositionColor>(device, m_basicEffect.get(),m_inputLayout.ReleaseAndGetAddressOf()); 
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void Line2D::Update(float deltaTime)
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
void Line2D::Draw(DirectX::CommonStates* pStates, float windowWidth, float windowHeight)
{
	const int VERTEX_NUM = 6 * 1;

	float halfSizeX = m_length / 2.0f;
	float halfSizeY = m_thickness / 2.0f;

	SimpleMath::Vector3 halfWindow = SimpleMath::Vector3(windowWidth / 2.0f, windowHeight / 2.0f, 0);


	VertexPositionColor vertexes[4] =
	{
		{halfWindow + SimpleMath::Vector3(-halfSizeX,  halfSizeY, 0.0f) , SimpleMath::Vector4(1.0f, 1.0f,1.0f,1.0f) }, // 左上
		{halfWindow + SimpleMath::Vector3(-halfSizeX, -halfSizeY, 0.0f) , SimpleMath::Vector4(1.0f, 1.0f,1.0f,1.0f) }, // 左下
		{halfWindow + SimpleMath::Vector3(+ halfSizeX,  -halfSizeY, 0.0f) , SimpleMath::Vector4(1.0f, 1.0f,1.0f,1.0f) }, // 右下
		{halfWindow + SimpleMath::Vector3(+halfSizeX,   halfSizeY, 0.0f) , SimpleMath::Vector4(1.0f, 1.0f,1.0f,1.0f) }, // 右上
	};


	const uint16_t indexes[VERTEX_NUM] =
	{
		0, 1, 3,
		3, 1, 2
	};
	auto context = m_pDeviceResources->GetD3DDeviceContext();


	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(pStates->DepthDefault(), 0);



	//	カリングは左周り
	context->RSSetState(pStates->CullNone());;

	// 画面のサイズに合わせた射影行列を設定
	m_basicEffect->SetProjection(SimpleMath::Matrix::CreateOrthographicOffCenter(
		0.0f,
		windowWidth,
		windowHeight,
		0.0f,
		0.0f,
		1.0f
	));

	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateRotationZ(m_radianAngle);

	// 平行移動行列を生成

	SimpleMath::Matrix defTranslation = SimpleMath::Matrix::CreateTranslation(-halfWindow);
	SimpleMath::Matrix translation = SimpleMath::Matrix::CreateTranslation(SimpleMath::Vector3(m_centerPosition));
	SimpleMath::Matrix worldMatrix = defTranslation * rotate  * translation;

	// ワールド行列を設定
	m_basicEffect->SetWorld(worldMatrix);
	m_basicEffect->SetView(SimpleMath::Matrix::Identity);

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	m_basicEffect->Apply(context);

	//	板ポリゴンを描画
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, VERTEX_NUM, vertexes, static_cast<size_t>(4));
	m_primitiveBatch->End();


}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Line2D::Finalize()
{

}

/**
 * @brief 中心座標
 * 
 * @param[in] center　中心座標
 */
void Line2D::SetPosition(const DirectX::SimpleMath::Vector2& center)
{
	m_centerPosition = center;
}

/**
 * @brief 長さの設定
 * 
 * @param[in] length　長さ
 */
void Line2D::SetLength(const float& length)
{
	m_length = length;
}

/**
 * @brief 太さの設定
 * 
 * @param[in] thickness　太さ
 */
void Line2D::SetThickness(const float& thickness)
{
	m_thickness = thickness;
}

/**
 * @brief 角度の設定
 * 
 * @param[in] radian　角度
 */
void Line2D::SetAngle(const float& radian)
{
	m_radianAngle = radian;
}
