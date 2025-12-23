/*****************************************************************//**
 * @file    EnemyFactory.h
 * @brief   敵の生成に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/Factory/FactoryBase.h"
#include "Game/GameObjects/Enemy/Enemy.h"


// クラスの前方宣言 ===================================================
class Enemy;
class Player;

namespace EnemyFactory
{
	/**
	 * @brief 追跡敵
	 */
	class TrackingEnemy
		: public FactoryBase<Enemy>
	{
	public:
		// コンストラクタ
		TrackingEnemy() = default;

		// デストラクタ
		~TrackingEnemy() = default;

	public:

		// 生成したオブジェクトを組み立てる
		void Assemble(Enemy* instance, const DefaultSpawnDesc& desc) override;
	};

	/**
	 * @brief 空中追跡敵
	 */
	class FlyingChaserEnemy
		: public FactoryBase<Enemy>
	{
	public:
		// コンストラクタ
		FlyingChaserEnemy() = default;

		// デストラクタ
		~FlyingChaserEnemy() = default;

	public:
		// 生成したオブジェクトを組み立てる
		void Assemble(Enemy* instance, const DefaultSpawnDesc& desc) override;

	};


}