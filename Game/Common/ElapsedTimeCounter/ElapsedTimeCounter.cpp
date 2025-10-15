/*****************************************************************//**
 * @file    ElapsedTimeCounter.h
 * @brief   経過時間カウンタクラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/13
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "ElapsedTimeCounter.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ElapsedTimeCounter::ElapsedTimeCounter()
	: m_deltaTime{ 0.0f }
{

}



/**
 * @brief デストラクタ
 */
ElapsedTimeCounter::~ElapsedTimeCounter()
{

}

/**
 * @brief 経過時間を加算
 * 
 * @param[in] deltaTime
 */
void ElapsedTimeCounter::UpperTime(float deltaTime)
{
	m_deltaTime += deltaTime;
}

/**
 * @brief 経過時間をリセット
 * 
 */
void ElapsedTimeCounter::Reset()
{
	m_deltaTime = 0.0f;
}



