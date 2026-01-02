/*****************************************************************//**
 * @file    DirectXMathUtils.h
 * @brief   DirectXTKの汎用的な数値ユーティリティに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/07/05
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "DirectXUtils.h"


/**
 * @brief 構造化バッファの作成
 *
 * @param[in] BindFlags        バッファのバインドフラグ（SRV/UAV など）
 * @param[in] Usage            バッファの使用方法（D3D11_USAGE_DEFAULT など）
 * @param[in] elementSize      1要素あたりのサイズ（構造体サイズ）
 * @param[in] count            要素数
 * @param[in] ppBuffer         作成されたバッファの出力先
 * @param[in] device           デバイス
 */
void MyLib::CreateStructuredBuffer(UINT BindFlags, D3D11_USAGE Usage, UINT elementSize, UINT count, ID3D11Buffer** ppBuffer, ID3D11Device1* device)
{
	// 構造化バッファ用の設定
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = BindFlags;                         // SRV/UAV として使用するか
	desc.Usage = Usage;                                 // CPU/GPU のアクセス方法
	desc.ByteWidth = elementSize * count;               // バッファ全体のサイズ
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファであることを指定
	desc.StructureByteStride = elementSize;             // 1要素のサイズ
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;       // CPU から書き込む場合に必要


	DX::ThrowIfFailed(
		device->CreateBuffer(&desc, nullptr, ppBuffer));
}

/**
 * @brief StructuredBuffer 用の SRV を作成
 *
 * @param[in] pSourceBuffer    元となるバッファ
 * @param[out] ppSRView        作成された SRV の出力先
 * @param[in] device           デバイス
 */
void MyLib::CreateBufferSRV(ID3D11Buffer* pSourceBuffer, ID3D11ShaderResourceView** ppSRView, ID3D11Device1* device)
{
	if (!pSourceBuffer || !ppSRView || !device) { return; }


	D3D11_BUFFER_DESC sourceBufferDesc{};
	pSourceBuffer->GetDesc(&sourceBufferDesc);

	// SRVのStructuredBuffer であることを確認
	if (!(sourceBufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) ||
		sourceBufferDesc.StructureByteStride == 0 ||
		sourceBufferDesc.BindFlags != D3D11_BIND_SHADER_RESOURCE) {
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = sourceBufferDesc.ByteWidth / sourceBufferDesc.StructureByteStride;
	desc.Buffer.FirstElement = 0;

	DX::ThrowIfFailed(
		device->CreateShaderResourceView(pSourceBuffer, &desc, ppSRView));
}

/**
 * @brief StructuredBuffer 用の UAV を作成
 *
 * @param[in] pSourceBuffer    元となるバッファ
 * @param[out] ppUAView        作成された UAV の出力先
 * @param[in] device           デバイス
 */
void MyLib::CreateBufferUAV(ID3D11Buffer* pSourceBuffer, ID3D11UnorderedAccessView** ppUAView, ID3D11Device1* device)
{
	if (!pSourceBuffer || !ppUAView || !device) { return; }


	D3D11_BUFFER_DESC sourceBufferDesc{};
	pSourceBuffer->GetDesc(&sourceBufferDesc);

	// UAVのStructuredBuffer であることを確認
	if (!(sourceBufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) ||
		sourceBufferDesc.StructureByteStride == 0 ||
		sourceBufferDesc.BindFlags != D3D11_BIND_UNORDERED_ACCESS) {
		return;
	}


	D3D11_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = sourceBufferDesc.ByteWidth / sourceBufferDesc.StructureByteStride;
	desc.Buffer.FirstElement = 0;


	DX::ThrowIfFailed(
		device->CreateUnorderedAccessView(pSourceBuffer, &desc, ppUAView));
}

/**
 * @brief GPU → CPU 転送用のステージングバッファを作成
 * 
 * @param[in] pSourceBuffer        元となる GPU バッファ
 * @param[out] ppStagingBuffer     作成されたステージングバッファ
 * @param[in] device               デバイス
 */

void MyLib::CreateStagingBuffer(ID3D11Buffer* pSourceBuffer, ID3D11Buffer** ppStagingBuffer, ID3D11Device1* device)
{
	// ソースバッファの情報を取得
	D3D11_BUFFER_DESC sourceDesc;
	pSourceBuffer->GetDesc(&sourceDesc);

	// ステージングバッファの作成
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sourceDesc.ByteWidth;          // サイズは元と同じ
	desc.Usage = D3D11_USAGE_STAGING;               // CPU 転送専用
	desc.BindFlags = 0;                             // パイプラインにバインドしない
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;    // CPU から読み取り可能

	desc.MiscFlags = 0;


	DX::ThrowIfFailed(
		device->CreateBuffer(&desc, nullptr, ppStagingBuffer));
}
/**
* @brief スクリーン座標からレイ情報へ計算する
*
* @param[in] screenX		スクリーン座標X
* @param[in] screenY		スクリーン座標Y
* @param[in] screenWidth	スクリーンの横幅
* @param[in] screenHeight	スクリーンの縦幅
* @param[in] eyePosition	カメラ座標
* @param[in] view			ビュー行列
* @param[in] projection	射影行列
* 
* @param[out] rayOrigin	レイの原点座標
* @param[out] rayDirection	レイの方向
*/
void MyLib::CalcScreenToWorldRay(
	const float& screenX,
	const float& screenY,

	const float& screenWidth,
	const float& screenHeight,

	const DirectX::SimpleMath::Vector3& eyePosition,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection,

	DirectX::SimpleMath::Vector3* rayDirection,
	DirectX::SimpleMath::Vector3* rayOrigin)
{

	using namespace DirectX;
	using namespace SimpleMath;

	// ビューポート変換行列を作成
	SimpleMath::Matrix viewportMatrix = SimpleMath::Matrix::Identity;
	viewportMatrix._11 = screenWidth / 2.0f;
	viewportMatrix._22 = -screenHeight / 2.0f;
	viewportMatrix._41 = screenWidth / 2.0f;
	viewportMatrix._42 = screenHeight / 2.0f;


	// ViewProjectionの逆行列
	Matrix invView = view.Invert();
	Matrix invProj = projection.Invert();
	Matrix invViewport = viewportMatrix.Invert();


	// スクリーンからワールド行列の変換用行列 (2D->3D) 
	SimpleMath::Matrix screenToWorldMatrix = invViewport * invProj * invView;

	// 遠方平面との交点を算出
	Vector3 point{ static_cast<float>(screenX), static_cast<float>(screenY), 1.0f };
	Vector3 intersectionFarPlane = SimpleMath::Vector3::Transform(point, screenToWorldMatrix);

	// カメラの座標を取り出す（行列からオフセットT部分の抽出）
	// {  1,  0,  0,  0}
	// {  0,  1,  0,  0}
	// {  0,  0,  1,  0}
	// { Tx,  Ty, Tz, 1}
	// Vector3 cameraPos = Vector3(view._41, view._42, view._43);
	Vector3 cameraPos = eyePosition;


	if (rayDirection)
	{
		// レイの方向を求める
		*rayDirection = intersectionFarPlane - cameraPos;
		// 正規化
		rayDirection->Normalize();
	}

	if (rayOrigin)
	{
		*rayOrigin = cameraPos;
	}

}