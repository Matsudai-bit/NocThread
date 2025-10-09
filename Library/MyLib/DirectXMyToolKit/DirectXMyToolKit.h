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
	 * @param[out] rayOrigin	レイの原点座標
	 * @param[out] rayDirection	レイの方向
	 */
	inline void CalcScreenToWorldRay(
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
};