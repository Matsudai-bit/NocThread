/*****************************************************************//**
 * @file    CollisionMatrix.h
 * @brief   衝突検知する対応付け表に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2026/01/03
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <unordered_map>
#include "Game/GameObjects/Common/GameObject.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 衝突判定対応付け表
 */
class CollisionMatrix
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	GameObjectTag m_matrixArray[32] ;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionMatrix();

	// デストラクタ
	~CollisionMatrix();


// 操作
public:
	// マトリックスのクリア
	void ClearMatrix();

	// 衝突を検知するかどうか
	bool ShouldCollide(const uint32_t& tagIndexA, const GameObjectTag& maskB) const;

	// 衝突検知対象の登録
	void RegisterDetectionTarget(const GameObjectTag& tagA, const GameObjectTag& tagB);
	


// 取得/設定
public:




// 内部実装
private:

	inline uint32_t GetBitIndex(uint32_t bit) const
	{
		unsigned long index;
		if (_BitScanForward(&index, bit)) return index;
		return 0;
	}
};
