/*****************************************************************//**
 * @file    MathUtils.h
 * @brief   汎用的な数値ユーティリティに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/05
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <cmath>


// クラスの前方宣言 ===================================================

/**
 * @brief 自作ライブラリ
 */
namespace MyLib
{
	//constexpr float DEFAULT_EPSILON = 1e-5f;


	/**
	 * @brief 値を決められた範囲に収める
	 *
	 * @param[in] val	 収める値
	 * @param[in] minVal 最大値
	 * @param[in] maxVal 最小値
	 *
	 * @return クランプ後の値
	 */
	template <typename T>
	inline T Clamp(T val, T minVal, T maxVal)
	{
		return std::max(minVal, std::min(val, maxVal));
	}

	/**
	 * @brief 浮動小数点数型の誤差を考慮に入れた比較
	 *
	 * @param[in] a 値A
	 * @param[in] b 値B
	 *
	 * @return 誤差を入れて等価かどうか
	 */
	template <typename T>
	inline bool ApproxEqual(T a, T b, float epsilon = 1e-5f)
	{
		return std::abs(a - b) <= epsilon;
	}


};