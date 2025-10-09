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
#include "Game/Common/Factory/EnemyFactory/IEnemyFactory/IEnemyFactory.h"


// クラスの前方宣言 ===================================================
class Enemy;
class Player;

namespace EnemyFactory
{
	/**
	 * @brief 追跡敵
	 */
	class TrackingEnemy
		: public IEnemyFactory
	{
	public:
		// コンストラクタ
		TrackingEnemy() = default;

		// デストラクタ
		~TrackingEnemy() = default;

	public:

		// 生成
		std::unique_ptr<Enemy> Create() override;

	};

	/**
	 * @brief 空中追跡敵
	 */
	class FlyingChaserEnemy
		: public IEnemyFactory
	{
	public:
		// コンストラクタ
		FlyingChaserEnemy() = default;

		// デストラクタ
		~FlyingChaserEnemy() = default;

	public:

		// 生成
		std::unique_ptr<Enemy> Create() override;

	};


}