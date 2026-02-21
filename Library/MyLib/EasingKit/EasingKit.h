/*****************************************************************//**
 * @file    Easing.h
 * @brief   イージングに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/29
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once


// 定数の定義 ===================================================

// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
#include <cmath>
#include <numbers>


namespace MyLib	// 自作ライブラリ
{

	/**
	 * @brief インサイン
	 *
	 * @param[in] value
	 * @return 値
	 */
	inline float EaseInSine(float value)
	{
		return 1.0f - std::cos((value * static_cast<float>(M_PI)) / 2.0f);
	}

	/**
	 * @brief アウトサイン
	 *
	 * @param[in] value
	 * @return 値
	 */
	inline float EaseOutSine(float value)
	{
		return std::sin((value * static_cast<float>(M_PI))) / 2.0f;
	}

/**
 * @brief アウトクイント
 *
 * @param[in] value
 * @return 値
 */
	inline float EaseOutQuint(float value)
	{
		return 1.0f - std::pow(1.0f - value, 5.0f);
	}

	/**
 * @brief アウトバック
 *
 * @param[in] value
 * @return　値
 */
	inline float EaseOutBack(float value)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;

		return 1.0f + c3 * std::pow(value - 1.0f, 3.0f) + c1 * std::pow(value - 1.0f, 2.0f);
	}

	/**
	 * @brief アウトバランス
	 * 
	 * @param[in] value
	 * @return　値 
	 */
	inline float EaseOutBounce(float value)
	{
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (value < 1.0f / d1)
			return n1 * value * value;
		else if (value < 2.0f / d1)
			return n1 * (value -= 1.5f / d1) * value + 0.75f;
		else if (value < 2.5f / d1)
			return n1 * (value -= 2.25f / d1) * value + 0.9375f;
		else
			return n1 * (value -= 2.625f / d1) * value + 0.984375f;
	}

	/**
	 * @brief イージング
	 * 
	 * @param[in] value値
	 * @return　値
	 */
	inline float EaseInOutQuint(float value)
	{
		return value < 0.5 ? 16 * value * value * value * value * value : 1.0f - std::pow(-2.0f * value + 2.0f, 5.0f) / 2.0f;
	}

	/**
 * @brief イージング
 *
* @param[in] value値
* @return　値
 */
	inline float EaseOutCubic(float value)
	{
		return 1.0f - (1- value) * (1 - value) * (1 - value);
	}
}