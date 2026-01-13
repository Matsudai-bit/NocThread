/*****************************************************************//**
 * @file    DirectXMathUtils.h
 * @brief   DirectXTK汎用的な数値ユーティリティに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/05
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <cmath>
#include "pch.h"


// クラスの前方宣言 ===================================================

/**
 * @brief 自作ライブラリ
 */
namespace MyLib
{
	// 構造化バッファの作成
	void CreateStructuredBuffer	(UINT BindFlags, D3D11_USAGE Usage, UINT elementSize, UINT count, ID3D11Buffer** ppBuffer, ID3D11Device1* device);
	// StructuredBuffer 用の SRV を作成
	void CreateBufferSRV		(ID3D11Buffer* pSourceBuffer, ID3D11ShaderResourceView** ppSRView,	ID3D11Device1* device);
	// StructuredBuffer 用の UAV を作成
	void CreateBufferUAV		(ID3D11Buffer* pSourceBuffer, ID3D11UnorderedAccessView** ppUAView, ID3D11Device1* device);
	// GPU → CPU 転送用のステージングバッファを作成
	void CreateStagingBuffer	(ID3D11Buffer* pSourceBuffer, ID3D11Buffer** ppStagingBuffer,		ID3D11Device1* device);

	// スクリーン座標からレイ情報へ計算する
	void CalcScreenToWorldRay(
		const float& screenX,
		const float& screenY,
		const float& screenWidth,
		const float& screenHeight,
		const DirectX::SimpleMath::Vector3& eyePosition,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection,
		DirectX::SimpleMath::Vector3* rayDirection,
		DirectX::SimpleMath::Vector3* rayOrigin);
	
};