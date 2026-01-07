/*****************************************************************//**
 * @file    DebugCamera.h
 * @brief   デバック用カメラクラス
 * 
 * @author  松下大暉
 * @date    2025/5/15
 *********************************************************************/

#include "Game/Common/Camera/Camera.h"
#pragma once

namespace MyLib
{

	// デバッグ用カメラクラス
	class DebugCamera : public Camera
	{
		// カメラの距離
		static const float DEFAULT_CAMERA_DISTANCE;

		// 横回転
		float m_yAngle, m_yTmp;

		// 縦回転
		float m_xAngle, m_xTmp;

		// ドラッグされた座標
		int m_x, m_y;

		float m_sx, m_sy;


		// スクロールフォイール値
		int m_scrollWheelValue;


		// マウストラッカー
		DirectX::Mouse::ButtonStateTracker m_tracker;

		// スクリーンサイズ
		int m_screenW, m_screenH;

	private:

		void Motion(int x, int y);

	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="windowWidth">ウインドウサイズ（幅）</param>
		/// <param name="windowHeight">ウインドウサイズ（高さ）</param>
		DebugCamera(int windowWidth, int windowHeight);

		/// <summary>
		/// デバッグカメラの更新
		/// </summary>
		/// <param name="mouse">マウスオブジェクトへのポインタ</param>
		void Update();

	

		/// <summary>
		/// 画面サイズの設定関数
		/// </summary>
		/// <param name="windowWidth">ウインドウサイズ（幅）</param>
		/// <param name="windowHeight">ウインドウサイズ（高さ）</param>
		void SetWindowSize(int windowWidth, int windowHeight);

		/// <summary>
		/// 画面サイズの取得関数
		/// </summary>
		void GetWindowSize(int& windowWidth, int& windowHeight);



	};
}

