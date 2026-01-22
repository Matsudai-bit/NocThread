/*****************************************************************//**
 * @file    CollisionMatrix.h
 * @brief   衝突判定対応付け表に関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/01/03
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionMatrix.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CollisionMatrix::CollisionMatrix()
	:m_matrixArray{}
{

}



/**
 * @brief デストラクタ
 */
CollisionMatrix::~CollisionMatrix()
{

}


void CollisionMatrix::ClearMatrix()
{
	
}

/**
 * @brief 衝突を検知するかどうか
 * 
 * @param[in] tagA  調べるタグA
 * @param[in] tagB	調べるタグB
 * 
 * @returns true　検知する
 * @returns false 検知しない
 */
bool CollisionMatrix::ShouldCollide(const uint32_t& tagIndexA, const GameObjectTag& maskB) const
{
	// ANDして0以外が帰ってきたら判定するペア
	return (static_cast<uint32_t>(m_matrixArray[tagIndexA] & maskB) != 0);
}

/**
 * @brief 衝突検知対象の登録
 * 
 * @param[in] tagA  検知するタグA
 * @param[in] tagB	検知するタグB
 */
void CollisionMatrix::RegisterDetectionTarget(const GameObjectTag& tagA, const GameObjectTag& tagB)
{
	uint32_t indexA = GetBitIndex(static_cast<uint32_t>(tagA));
	m_matrixArray[indexA] = m_matrixArray[indexA] | tagB;

	uint32_t indexB = GetBitIndex(static_cast<uint32_t>(tagB));
	m_matrixArray[indexB] = m_matrixArray[indexB] | tagA;

}

