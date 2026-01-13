/*****************************************************************//**
 * @file    TransitionMask.cpp
 * @brief   画面切り替え用のマスク表示クラス
 * 
 * @author  松下大暉
 * @date    2025/12/12
 * 
 *********************************************************************/
#include "pch.h"
#include "TransitionMask.h"
#include "Library/DirectXFramework/ReadData.h"

using namespace DirectX;

// コンストラクタ
TransitionMask::TransitionMask(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	float interval
)
	: m_interval(interval)
	, m_rate(0.0f)
	, m_open(true)
	, m_request{}
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps_fade = DX::ReadData(L"Resources/Shaders/PS_Fade.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_fade.data(), ps_fade.size(), nullptr, m_PS_Fade.ReleaseAndGetAddressOf())
	);

	//// マスク用テクスチャの作成
	//DX::ThrowIfFailed(
	//	CreateDDSTextureFromFile(
	//		device,
	//		L"Resources/Textures/FadeMask.dds",
	//		nullptr,
	//		m_maskTexture.ReleaseAndGetAddressOf()
	//	)
	//);

	D3D11_BUFFER_DESC bufferDesk = {};
	bufferDesk.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));
	bufferDesk.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesk.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesk.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesk, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
	);
}

// 更新処理
void TransitionMask::Update(float elapsedTime)
{
	if (m_open)
	{
		// オープン
		// (減算量をインターバルで割る＝インターバル分の時間にする)
		// ０以下になったら０にする
		m_rate -= elapsedTime / m_interval;	
		m_rate = std::max(m_rate, 0.0f);

	}
	else
	{
		// クローズ
		// (加算量をインターバルで割る＝インターバル分の時間にする)
		// 1以上になったら１にする
		m_rate += elapsedTime / m_interval;
		m_rate = std::min(m_rate, 1.0f);
		if (m_rate >= 1.0f)
		{
			if (m_onFinish)
			{
				m_onFinish();
				m_onFinish = nullptr;
			}
		}

	}
}


// 描画処理
void TransitionMask::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	ID3D11ShaderResourceView* texture,
	const RECT& rect
)
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto data = static_cast<ConstantBuffer*>(mappedResource.pData);
	data->rate = m_rate;
	context->Unmap(m_constantBuffer.Get(), 0);

	m_spriteBatch->Begin(
		SpriteSortMode_Immediate, states->NonPremultiplied(), nullptr, nullptr, nullptr,
		[&](){

			// 定数バッファの作成
			ID3D11Buffer* cbuff = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, &cbuff);

			//// マスク用テクスチャ
			//context->PSSetShaderResources(1, 1, m_maskTexture.GetAddressOf());

			// ピクセルシェーダ
			context->PSSetShader(m_PS_Fade.Get(), nullptr, 0);
		}
	);

	// マスクの描画
	m_spriteBatch->Draw(texture, rect);

	m_spriteBatch->End();
}

// マスクをオープンする関数
void TransitionMask::Open()
{
	m_open = true;
	m_rate = 1.0f;
}

// マスクをクローズする関数
void TransitionMask::Close(FinishFucntion onFinish)
{
	m_open = false;
	m_rate = 0.0f;

	m_onFinish = onFinish;
}

