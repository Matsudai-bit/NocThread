/*****************************************************************//**
 * @file    Ray.h
 * @brief   レイに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/07/02
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Ray.h"




// メンバ関数の定義 ===========================================================



/**
 * @brief コンストラクタ
 * 
 * @param[in] origin　原点
 * @param[in] direction　方向
 */
MyLib::Ray::Ray(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& direction)
	: direction{ direction }
	, origin{ origin }
{


}

/**
 * @brief デストラクタ
 */
MyLib::Ray::~Ray()
{

}


