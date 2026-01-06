/*****************************************************************//**
 * @file    TransitionMask.h
 * @brief   画面切り替え用のマスク表示クラス
 * 
 * @author  松下大暉
 * @date    2025/12/12
 *********************************************************************/
#pragma once

#include "SpriteBatch.h"

class TransitionMask
{

// エイリアス宣言
private :

	using FinishFucntion = std::function<void()>;

public:

	enum class CreateMaskRequest
	{
		NONE,
		COPY,
	};


private:

	// 割合(0～1)
	float m_rate;

	// オープン又はクローズするまでの時間
	float m_interval;

	// オープンフラグ（trueの場合オープン）
	bool m_open;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	CreateMaskRequest m_request;

	struct ConstantBuffer
	{
		float rate;
		float pad[3];
	};

	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PS_Fade;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_maskTexture;

	FinishFucntion m_onFinish;	///< 遷移終了時に呼ばれる

public:

public:

	// コンストラクタ
	TransitionMask(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		float interval
	);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		ID3D11ShaderResourceView* texture,
		const RECT& rect
	);


	// オープンする関数
	void Open();

	// クローズする関数
	void Close(FinishFucntion onFinish);

	// オープン中かチェックする関数
	bool IsOpen() const { return m_open; };

	// クローズ中かチェックする関数
	bool IsClose() const { return !m_open; }

	// オープン具合を返す関数(0～1)
	float GetOpenRate() { return m_rate; }

	// オープン、クローズまでの時間を設定する関数
	void SetInterval(float interval) { m_interval = interval; }

	void SetCreateMaskRequest(CreateMaskRequest request) 
	{
		m_request = request; 

	}
	CreateMaskRequest GetCreateMaskRequest()  const { return m_request ; }


	float GetRate()  const
	{  
		if (m_open) return (1.0f - m_rate);
		return  m_rate;
	}

	bool IsEnd() const
	{
		if (GetRate() == 1.0f) return true;
		return false;
	}
};
