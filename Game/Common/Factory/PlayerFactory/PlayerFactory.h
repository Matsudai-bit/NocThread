/*****************************************************************//**
 * @file    PlayerFactory.h
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
class Player;

namespace PlayerFactory
{
	struct PlayerDesk
	{

	};

	/**
	 * @brief 追跡敵
	 */
	class StagePlayer
		: public FactoryBase<Player, PlayerDesk>
	{
	public:
		// コンストラクタ
		StagePlayer() = default;

		// デストラクタ
		~StagePlayer() = default;

	public:

		// 生成したオブジェクトを組み立てる
		void Assemble(Player* instance, const PlayerDesk& desc) override;
	};

	


}