/*****************************************************************//**
 * @file    OffscreenRendering.h
 * @brief   オフスクリーンレンダリングに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/11/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "OffscreenRendering.h"

#include "Library/DirectXFramework/RenderTexture.h"
#include "Library/DirectXFramework/DeviceResources.h"
#include "Library/MyLib/DirectXMyToolKit/DepthStencil/DepthStencil.h"

// エイリアス宣言
using namespace MyLib;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
OffscreenRendering::OffscreenRendering(
	DX::DeviceResources* pDeviceResources,
	DXGI_FORMAT renderTextureFormat, 
	DXGI_FORMAT depthStencilFormat)
{
	m_pDeviceResources = pDeviceResources;

	// レンダーテクスチャの作成
	m_offscreenRT = std::make_unique<DX::RenderTexture>		(renderTextureFormat);
	// デプスステンシルの作成
	m_offscreenDS = std::make_unique<MyLib::DepthStencil>	(depthStencilFormat);
}



/**
 * @brief デストラクタ
 */
OffscreenRendering::~OffscreenRendering()
{

}

/**
 * @brief 開始
 * 
 * @param[in] screenColor　背景色
 */
void MyLib::OffscreenRendering::Begin(const DirectX::SimpleMath::Color& screenColor)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	auto rtv = m_offscreenRT->GetRenderTargetView();
	auto dsv = m_offscreenDS->GetDepthStencilView();

	context->ClearRenderTargetView(rtv, screenColor);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
	// レンダーターゲットの切り替え
	context->OMSetRenderTargets(1, &rtv, dsv);
}

/**
 * @brief 終了
 * 
 * 元のレンダーターゲットに戻す
 */
void MyLib::OffscreenRendering::End()
{
	// 処理用変数
	auto context = m_pDeviceResources->GetD3DDeviceContext();
	auto renderTarget = m_pDeviceResources->GetRenderTargetView();
	auto depthStencil = m_pDeviceResources->GetDepthStencilView();

	// 元のレンダーターゲットに戻す
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
}

/**
 * @brief デバイスの設定
 * 
 * @param[in] device　デバイス
 */
void MyLib::OffscreenRendering::SetDevice(ID3D11Device* device)
{
	m_offscreenRT->SetDevice(device);
	m_offscreenDS->SetDevice(device);
}

/**
 * @brief ウィンドウサイズの設定
 * 
 * @param[in] rect　ウィンドウサイズ
 */
void MyLib::OffscreenRendering::SetWindow(const RECT& rect)
{
	m_offscreenRT->SetWindow(rect);
	m_offscreenDS->SetWindow(rect);
}

/**
 * @brief ShaderResourceViewの取得
 * 
 * @return 描画したテクスチャの取得
 */
ID3D11ShaderResourceView* MyLib::OffscreenRendering::GetShaderResourceView() const
{
	return m_offscreenRT->GetShaderResourceView();
}


