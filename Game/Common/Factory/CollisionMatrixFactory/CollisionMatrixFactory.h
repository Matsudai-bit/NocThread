/*****************************************************************//**
 * @file    CollisionMatrixFactory.h
 * @brief   衝突検知表のファクトリーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2026/01/03
 *********************************************************************/

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/Factory/FactoryBase.h"

// クラスの前方宣言 ===================================================
class CollisionMatrix;

namespace CollisionMatrixFactory
{
	/**
	 * @brief 追跡敵
	 */
	class StageCollisionMatrix
		: public FactoryBase<CollisionMatrix>
	{
	public:
		// コンストラクタ
		StageCollisionMatrix() = default;

		// デストラクタ
		~StageCollisionMatrix() = default;

	protected:

		// 生成したオブジェクトを組み立てる
		void Assemble(CollisionMatrix* instance, const DefaultSpawnDesc& desc) override;
	};

	


}