/*****************************************************************//**
 * @file    OffscreenRendering.h
 * @brief   オフスクリーンレンダリングに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/11/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================

// クラスの前方宣言 ===================================================

// DirectXTK関連ライブラリ
namespace DX
{
	class RenderTexture;
	class DeviceResources;
}

// 自作ライブラリ
namespace MyLib
{
	class DepthStencil;
}


namespace MyLib
{
	// クラスの定義 ===============================================================
	/**
	 * @brief オフスクリーンレンダリング
	 */
	class OffscreenRendering
	{
		// クラス定数の宣言 -------------------------------------------------
	public:



		// データメンバの宣言 -----------------------------------------------
	private:

		DX::DeviceResources* m_pDeviceResources; 
		std::unique_ptr<DX::RenderTexture>		m_offscreenRT; // ミニマップの描画テクスチャ
		std::unique_ptr<MyLib::DepthStencil>	m_offscreenDS; // ミニマップのデプスステンシル

		// メンバ関数の宣言 -------------------------------------------------
		// コンストラクタ/デストラクタ
	public:
		// コンストラクタ
		OffscreenRendering(
			DX::DeviceResources* pDeviceResources,
			DXGI_FORMAT renderTextureFormat, 
			DXGI_FORMAT depthStencilFormat);
		// デストラクタ
		~OffscreenRendering();


		// 操作
	public:

		// 開始（レンダーターゲットを変更する）
		void Begin(const DirectX::SimpleMath::Color& screenColor);
		// 終了(元のレンダーターゲットに戻す)
		void End();


		// 取得/設定
	public:
		// デバイスの設定
		void SetDevice(ID3D11Device* device);
		// ウィンドウサイズの設定
		void SetWindow(const RECT& rect);

		// ShaderResourceViewの取得
		ID3D11ShaderResourceView* GetShaderResourceView() const;

		// 内部実装
	private:


	};
}