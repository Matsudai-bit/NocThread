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
	constexpr float DEFAULT_EPSILON = 1e-5f;

	/**
	 * @brief 浮動小数点数型の誤差を考慮に入れた比較
	 *
	 * @param[in] a 値A
	 * @param[in] b 値B
	 *
	 * @return 誤差を入れて等価かどうか
	 */
	inline bool ApproxEqual(const DirectX::SimpleMath::Vector2& a,const DirectX::SimpleMath::Vector2& b,float epsilon = DEFAULT_EPSILON)
	{
		return std::abs(a.x - b.x) <= epsilon &&
			std::abs(a.y - b.y) <= epsilon;
	}


	/**
	* @brief 浮動小数点数型の誤差を考慮に入れた比較
	*
	* @param[in] a 値A
	* @param[in] b 値B
	*
	* @return 誤差を入れて等価かどうか
	*/
	inline bool ApproxEqual(const DirectX::SimpleMath::Vector3& a, const DirectX::SimpleMath::Vector3& b, float epsilon = DEFAULT_EPSILON)
	{
		return std::abs(a.x - b.x) <= epsilon &&
			std::abs(a.y - b.y) <= epsilon &&
			std::abs(a.z - b.z) <= epsilon;
	}
};